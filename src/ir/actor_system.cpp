/**
 * @file actor_system.cpp
 * @brief Actor 系统实现
 */

#include "syclang/ir/actor_system.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

namespace syclang {
namespace ir {

// ============================================================================
// ActorRef 实现
// ============================================================================

ActorRef::ActorRef(const std::string& path, const std::string& name)
    : path_(path), name_(name) {}

template<typename T>
std::future<T> ActorRef::send_async(const std::string& message_name, const T& data) {
    // 序列化数据
    std::vector<uint8_t> serialized_data(sizeof(T));
    std::memcpy(serialized_data.data(), &data, sizeof(T));
    
    // 查找 Actor
    auto& system = ActorSystem::instance();
    auto actor = system.find_actor(path_);
    
    if (!actor) {
        throw std::runtime_error("Actor not found: " + path_);
    }
    
    // 创建 promise 用于异步结果
    auto promise = std::make_shared<std::promise<std::vector<uint8_t>>>();
    auto future = promise->get_future();
    
    // 发送消息
    auto message = std::make_shared<ActorMessage>(ActorMessageType::NORMAL, message_name, serialized_data);
    actor->send_message(message);
    
    return future;
}

template<typename T>
T ActorRef::send_sync(const std::string& message_name, const T& data) {
    auto future = send_async<T>(message_name, data);
    auto result_data = future.get();
    
    if (result_data.size() != sizeof(T)) {
        throw std::runtime_error("Response size mismatch");
    }
    
    T result;
    std::memcpy(&result, result_data.data(), sizeof(T));
    return result;
}

template<typename T>
void ActorRef::send(const std::string& message_name, const T& data) {
    std::vector<uint8_t> serialized_data(sizeof(T));
    std::memcpy(serialized_data.data(), &data, sizeof(T));
    
    auto& system = ActorSystem::instance();
    auto actor = system.find_actor(path_);
    
    if (!actor) {
        throw std::runtime_error("Actor not found: " + path_);
    }
    
    auto message = std::make_shared<ActorMessage>(ActorMessageType::NORMAL, message_name, serialized_data);
    actor->send_message(message);
}

// ============================================================================
// Actor 实现
// ============================================================================

Actor::Actor(const std::string& name, const ActorMailboxConfig& config)
    : name_(name), config_(config), state_(ActorState::CREATED) {}

Actor::~Actor() {
    if (worker_thread_.joinable()) {
        stop();
    }
}

void Actor::start() {
    if (state_ != ActorState::CREATED && state_ != ActorState::STOPPED) {
        throw std::runtime_error("Actor already running or in transition state");
    }
    
    state_ = ActorState::STARTING;
    worker_thread_ = std::thread(&Actor::run_loop, this);
    
    // 等待 Actor 进入运行状态
    while (state_ != ActorState::RUNNING) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void Actor::stop() {
    if (state_ != ActorState::RUNNING) {
        return;
    }
    
    state_ = ActorState::STOPPING;
    mailbox_cv_.notify_all();
    
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
    
    state_ = ActorState::STOPPED;
}

void Actor::send_message(std::shared_ptr<ActorMessage> message) {
    std::unique_lock<std::mutex> lock(mailbox_mutex_);
    
    if (config_.drop_when_full && mailbox_.size() >= config_.capacity) {
        std::cerr << "Actor mailbox full, dropping message: " << message->message_name << std::endl;
        return;
    }
    
    // 等待邮箱有空间
    auto timeout = std::chrono::milliseconds(config_.timeout_ms);
    if (!mailbox_cv_.wait_for(lock, timeout, [this] {
        return mailbox_.size() < config_.capacity || state_ != ActorState::RUNNING;
    })) {
        throw std::runtime_error("Actor mailbox timeout");
    }
    
    mailbox_.push(message);
    mailbox_cv_.notify_one();
}

ActorRef Actor::get_ref() {
    return ActorRef("/" + name_, name_);
}

void Actor::run_loop() {
    state_ = ActorState::RUNNING;
    
    try {
        process_messages();
    } catch (const std::exception& e) {
        std::cerr << "Actor " << name_ << " error: " << e.what() << std::endl;
    }
    
    state_ = ActorState::STOPPED;
}

void Actor::process_messages() {
    while (state_ == ActorState::RUNNING) {
        std::shared_ptr<ActorMessage> message;
        
        {
            std::unique_lock<std::mutex> lock(mailbox_mutex_);
            
            // 等待消息
            mailbox_cv_.wait(lock, [this] {
                return !mailbox_.empty() || state_ != ActorState::RUNNING;
            });
            
            if (state_ != ActorState::RUNNING) {
                break;
            }
            
            message = mailbox_.front();
            mailbox_.pop();
        }
        
        if (message) {
            on_message(message->message_name, message->data);
        }
    }
}

// ============================================================================
// ActorSystem 实现
// ============================================================================

ActorSystem::ActorSystem() : running_(false) {}

ActorSystem::~ActorSystem() {
    shutdown();
}

ActorSystem& ActorSystem::instance() {
    static ActorSystem instance;
    return instance;
}

template<typename T, typename... Args>
ActorRef ActorSystem::create_actor(const std::string& name, Args&&... args) {
    std::lock_guard<std::mutex> lock(actors_mutex_);
    
    auto actor = std::make_shared<T>(name, std::forward<Args>(args)...);
    actor->start();
    
    std::string path = "/" + name;
    actors_[path] = actor;
    
    return actor->get_ref();
}

std::shared_ptr<Actor> ActorSystem::find_actor(const std::string& path) {
    std::lock_guard<std::mutex> lock(actors_mutex_);
    
    auto it = actors_.find(path);
    if (it != actors_.end()) {
        return it->second;
    }
    
    return nullptr;
}

void ActorSystem::shutdown() {
    std::lock_guard<std::mutex> lock(actors_mutex_);
    
    running_ = false;
    
    for (auto& pair : actors_) {
        pair.second->stop();
    }
    
    actors_.clear();
}

void ActorSystem::broadcast(const std::string& message_name, const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(actors_mutex_);
    
    for (auto& pair : actors_) {
        auto message = std::make_shared<ActorMessage>(ActorMessageType::NORMAL, message_name, data);
        pair.second->send_message(message);
    }
}

// ============================================================================
// DistributedLock 实现
// ============================================================================

DistributedLock::DistributedLock(const std::string& lock_name, int timeout_ms)
    : lock_name_(lock_name), timeout_ms_(timeout_ms), locked_(false) {}

bool DistributedLock::try_lock() {
    // 这里应该调用分布式锁服务（如 Redis、etcd、ZooKeeper）
    // 简化实现：使用本地文件模拟
    
    std::string lock_file = "/tmp/" + lock_name_ + ".lock";
    
    struct stat st;
    if (stat(lock_file.c_str(), &st) == 0) {
        // 文件存在，检查是否超时
        auto current_time = std::chrono::system_clock::now();
        auto file_time = std::chrono::system_clock::from_time_t(st.st_mtime);
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - file_time).count();
        
        if (elapsed < timeout_ms_) {
            return false;
        }
    }
    
    // 创建锁文件
    FILE* f = fopen(lock_file.c_str(), "w");
    if (!f) {
        return false;
    }
    fclose(f);
    
    locked_ = true;
    return true;
}

void DistributedLock::lock() {
    while (!try_lock()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void DistributedLock::unlock() {
    if (!locked_) {
        return;
    }
    
    std::string lock_file = "/tmp/" + lock_name_ + ".lock";
    std::remove(lock_file.c_str());
    
    locked_ = false;
}

DistributedLock::LockGuard::LockGuard(DistributedLock& lock) : lock_(lock) {
    lock_.lock();
}

DistributedLock::LockGuard::~LockGuard() {
    lock_.unlock();
}

// ============================================================================
// RPCService 实现
// ============================================================================

RPCService::RPCService(const std::string& service_name,
                       SerializationFormat format,
                       TransportProtocol protocol)
    : service_name_(service_name), serialization_format_(format), transport_protocol_(protocol), running_(false) {}

RPCService::~RPCService() {
    stop();
}

template<typename R, typename... Args>
void RPCService::register_method(const std::string& method_name,
                                 std::function<R(Args...)> handler) {
    methods_[method_name] = [handler](const std::vector<uint8_t>& input) -> std::vector<uint8_t> {
        // 反序列化参数
        std::tuple<Args...> args;
        // 这里应该实现实际的反序列化逻辑
        
        // 调用处理函数
        R result = std::apply(handler, args);
        
        // 序列化结果
        std::vector<uint8_t> output(sizeof(R));
        std::memcpy(output.data(), &result, sizeof(R));
        
        return output;
    };
}

void RPCService::start(const std::string& address, int port) {
    running_ = true;
    
    std::string bind_addr = address + ":" + std::to_string(port);
    std::cout << "RPC Service " << service_name_ << " started on " << bind_addr << std::endl;
    
    // 这里应该启动实际的 RPC 服务（如 gRPC 服务器）
    // 简化实现：使用 HTTP 模拟
}

void RPCService::stop() {
    running_ = false;
    std::cout << "RPC Service " << service_name_ << " stopped" << std::endl;
}

} // namespace ir
} // namespace syclang
