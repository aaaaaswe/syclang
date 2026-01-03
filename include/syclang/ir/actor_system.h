/**
 * @file actor_system.h
 * @brief Actor 模型支持 - SysLang v3.0 分布式系统原语
 * 
 * 支持中文关键字：演员(Actor)、发送(Send)、接收(Receive)
 * 
 * SysLang v3.0 新增功能：Actor模型实现
 */

#ifndef SYCLANG_IR_ACTOR_SYSTEM_H
#define SYCLANG_IR_ACTOR_SYSTEM_H

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <future>
#include <queue>
#include <unordered_map>
#include <atomic>

namespace syclang {
namespace ir {

/**
 * @brief Actor 消息类型
 */
enum class ActorMessageType {
    NORMAL,
    SYSTEM,
    CONTROL
};

/**
 * @brief Actor 消息基类
 */
struct ActorMessage {
    ActorMessageType type;
    std::string message_name;
    std::vector<uint8_t> data;
    
    ActorMessage(ActorMessageType t, const std::string& name, const std::vector<uint8_t>& d)
        : type(t), message_name(name), data(d) {}
};

/**
 * @brief Actor 邮箱配置
 */
struct ActorMailboxConfig {
    size_t capacity;          // 邮箱容量
    int timeout_ms;           // 超时时间（毫秒）
    bool drop_when_full;      // 邮箱满时是否丢弃
    
    ActorMailboxConfig()
        : capacity(1000), timeout_ms(5000), drop_when_full(false) {}
};

/**
 * @brief Actor 状态
 */
enum class ActorState {
    CREATED,
    STARTING,
    RUNNING,
    STOPPING,
    STOPPED
};

/**
 * @brief Actor 引用
 */
class ActorRef {
public:
    ActorRef(const std::string& path, const std::string& name);
    
    // 异步发送消息
    template<typename T>
    std::future<T> send_async(const std::string& message_name, const T& data);
    
    // 同步发送消息（等待响应）
    template<typename T>
    T send_sync(const std::string& message_name, const T& data);
    
    // 发送单向消息
    template<typename T>
    void send(const std::string& message_name, const T& data);
    
    std::string get_path() const { return path_; }
    std::string get_name() const { return name_; }
    
private:
    std::string path_;
    std::string name_;
};

/**
 * @brief Actor 基类
 */
class Actor {
public:
    Actor(const std::string& name, const ActorMailboxConfig& config);
    virtual ~Actor();
    
    // 启动 Actor
    void start();
    
    // 停止 Actor
    void stop();
    
    // 发送消息到 Actor
    void send_message(std::shared_ptr<ActorMessage> message);
    
    // 获取 Actor 引用
    ActorRef get_ref();
    
    // 消息处理接口（子类实现）
    virtual void on_message(const std::string& message_name, const std::vector<uint8_t>& data) = 0;
    
protected:
    // 发送回复
    template<typename T>
    void reply(const T& response);
    
    // 记录状态
    std::atomic<ActorState> state_;
    ActorMailboxConfig config_;
    
private:
    void run_loop();
    void process_messages();
    
    std::string name_;
    std::queue<std::shared_ptr<ActorMessage>> mailbox_;
    std::mutex mailbox_mutex_;
    std::condition_variable mailbox_cv_;
    std::thread worker_thread_;
    std::unordered_map<std::string, std::promise<std::vector<uint8_t>>> pending_replies_;
    std::mutex replies_mutex_;
};

/**
 * @brief Actor 系统管理器
 */
class ActorSystem {
public:
    static ActorSystem& instance();
    
    // 创建 Actor
    template<typename T, typename... Args>
    ActorRef create_actor(const std::string& name, Args&&... args);
    
    // 查找 Actor
    std::shared_ptr<Actor> find_actor(const std::string& path);
    
    // 停止所有 Actor
    void shutdown();
    
    // 广播消息
    void broadcast(const std::string& message_name, const std::vector<uint8_t>& data);
    
private:
    ActorSystem();
    ~ActorSystem();
    
    ActorSystem(const ActorSystem&) = delete;
    ActorSystem& operator=(const ActorSystem&) = delete;
    
    std::unordered_map<std::string, std::shared_ptr<Actor>> actors_;
    std::mutex actors_mutex_;
    std::atomic<bool> running_;
};

/**
 * @brief 分布式锁
 */
class DistributedLock {
public:
    DistributedLock(const std::string& lock_name, int timeout_ms = 30000);
    
    // 尝试获取锁
    bool try_lock();
    
    // 获取锁（阻塞）
    void lock();
    
    // 释放锁
    void unlock();
    
    // RAII 风格的锁守卫
    class LockGuard {
    public:
        LockGuard(DistributedLock& lock);
        ~LockGuard();
        
    private:
        DistributedLock& lock_;
    };
    
private:
    std::string lock_name_;
    int timeout_ms_;
    bool locked_;
};

/**
 * @brief RPC 服务基类
 */
class RPCService {
public:
    enum class SerializationFormat {
        JSON,
        ProtocolBuffers,
        MessagePack,
        CBOR
    };
    
    enum class TransportProtocol {
        gRPC,
        HTTP,
        WebSocket,
        ZeroMQ
    };
    
    RPCService(const std::string& service_name,
               SerializationFormat format,
               TransportProtocol protocol);
    
    virtual ~RPCService();
    
    // 注册 RPC 方法
    template<typename R, typename... Args>
    void register_method(const std::string& method_name,
                         std::function<R(Args...)> handler);
    
    // 启动服务
    void start(const std::string& address, int port);
    
    // 停止服务
    void stop();
    
protected:
    // 远程调用
    template<typename R, typename... Args>
    R call_remote(const std::string& service_name,
                  const std::string& method_name,
                  Args&&... args);
    
private:
    std::string service_name_;
    SerializationFormat serialization_format_;
    TransportProtocol transport_protocol_;
    std::unordered_map<std::string, std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)>> methods_;
    std::atomic<bool> running_;
};

} // namespace ir
} // namespace syclang

#endif // SYCLANG_IR_ACTOR_SYSTEM_H
