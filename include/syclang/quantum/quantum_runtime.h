/**
 * @file quantum_runtime.h
 * @brief 量子计算运行时 - SysLang v4.0
 * 
 * 提供量子计算的原语和运行时支持
 * 
 * SysLang v4.0 新增功能：量子计算支持
 */

#ifndef SYCLANG_QUANTUM_QUANTUM_RUNTIME_H
#define SYCLANG_QUANTUM_QUANTUM_RUNTIME_H

#include <complex>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <random>

namespace syclang {
namespace quantum {

// 复数类型
using Complex = std::complex<double>;

/**
 * @brief 量子态向量
 */
struct QuantumState {
    std::vector<Complex> amplitudes;
    size_t num_qubits;
    
    QuantumState(size_t n_qubits);
    
    // 应用量子门
    void apply_gate(const std::vector<std::vector<Complex>>& gate,
                    const std::vector<size_t>& qubits);
    
    // 测量量子位
    int measure_qubit(size_t qubit);
    
    // 获取测量概率
    double get_probability(size_t qubit, int value);
};

/**
 * @brief 量子门类型
 */
enum class QuantumGateType {
    // 单量子比特门
    PAULI_X,      // X 门
    PAULI_Y,      // Y 门
    PAULI_Z,      // Z 门
    HADAMARD,     // 哈达玛门
    PHASE,        // 相位门
    RX,           // X 旋转门
    RY,           // Y 旋转门
    RZ,           // Z 旋转门
    T,            // T 门
    S,            // S 门
    
    // 双量子比特门
    CNOT,         // 控制非门
    CX,           // 控制X门（同CNOT）
    CZ,           // 控制Z门
    SWAP,         // 交换门
    ISWAP,        // iSWAP 门
    TOFFOLI,      // Toffoli 门（三量子比特）
    FREDKIN,      // Fredkin 门（三量子比特）
    
    // 多量子比特门
    FOURIER_TRANSFORM,  // 量子傅里叶变换
    GROVER_ORACLE,     // Grover 算法预言机
    PHASE_ESTIMATION   // 相位估计
};

/**
 * @brief 量子门
 */
class QuantumGate {
public:
    QuantumGate(QuantumGateType type);
    
    // 设置参数（用于参数化门）
    void set_parameter(double theta);
    void set_parameters(const std::vector<double>& params);
    
    // 获取门矩阵
    std::vector<std::vector<Complex>> get_matrix(size_t num_qubits) const;
    
    // 获取门类型
    QuantumGateType get_type() const { return type_; }
    
private:
    QuantumGateType type_;
    std::vector<double> parameters_;
    
    // 生成单量子比特门矩阵
    std::vector<std::vector<Complex>> single_qubit_gate_matrix() const;
    
    // 生成双量子比特门矩阵
    std::vector<std::vector<Complex>> two_qubit_gate_matrix() const;
    
    // 生成三量子比特门矩阵
    std::vector<std::vector<Complex>> three_qubit_gate_matrix() const;
};

/**
 * @brief 量子电路
 */
class QuantumCircuit {
public:
    QuantumCircuit(size_t num_qubits);
    ~QuantumCircuit();
    
    // 添加门
    void add_gate(const QuantumGate& gate, const std::vector<size_t>& qubits);
    
    // 常用门快捷方法
    void h(size_t qubit);           // 哈达玛门
    void x(size_t qubit);           // X 门
    void y(size_t qubit);           // Y 门
    void z(size_t qubit);           // Z 门
    void cnot(size_t control, size_t target);
    void swap(size_t qubit1, size_t qubit2);
    void rx(size_t qubit, double theta);
    void ry(size_t qubit, double theta);
    void rz(size_t qubit, double theta);
    
    // 测量
    int measure_qubit(size_t qubit);
    std::vector<int> measure_all();
    
    // 执行电路
    QuantumResult execute();
    
    // 获取量子态
    QuantumState get_state();
    
    // 深度优化
    void optimize();
    
    // 可视化
    std::string visualize() const;
    
    // 保存/加载
    void save_qasm(const std::string& filename) const;
    void save_quil(const std::string& filename) const;
    
private:
    size_t num_qubits_;
    std::vector<std::pair<QuantumGate, std::vector<size_t>>> gates_;
    QuantumState state_;
    bool executed_;
};

/**
 * @brief 量子结果
 */
struct QuantumResult {
    std::map<std::string, int> measurements;  // 测量结果计数
    std::vector<int> final_state;             // 最终量子态
    std::vector<double> probabilities;       // 各态概率
    size_t shots;                             // 重复次数
};

/**
 * @brief 量子模拟器
 */
class QuantumSimulator {
public:
    enum class Backend {
        STATEVECTOR,    // 状态向量模拟器
        STABILIZER,     // 稳定子模拟器
        TENSOR_NETWORK, // 张量网络模拟器
        MPS             // 矩阵乘积态模拟器
    };
    
    QuantumSimulator(Backend backend = Backend::STATEVECTOR);
    ~QuantumSimulator();
    
    // 运行电路
    QuantumResult run(QuantumCircuit& circuit, size_t shots = 1000);
    
    // 配置
    void set_backend(Backend backend);
    void set_precision(double precision);
    void set_max_qubits(size_t max_qubits);
    
    // 性能统计
    void enable_profiling(bool enable);
    std::map<std::string, double> get_performance_stats() const;
    
private:
    Backend backend_;
    double precision_;
    size_t max_qubits_;
    bool profiling_enabled_;
    std::map<std::string, double> performance_stats_;
};

/**
 * @brief 量子算法
 */
namespace algorithms {

/**
 * @brief Grover 搜索算法
 */
class GroverAlgorithm {
public:
    GroverAlgorithm(size_t num_qubits, const std::vector<int>& marked_items);
    
    // 构建 Grover 电路
    QuantumCircuit build_circuit();
    
    // 执行搜索
    std::vector<int> search(size_t shots = 1000);
    
private:
    size_t num_qubits_;
    std::vector<int> marked_items_;
    
    // 构建预言机
    void build_oracle(QuantumCircuit& circuit);
    
    // 构建扩散算子
    void build_diffusion(QuantumCircuit& circuit);
    
    // 计算最优迭代次数
    size_t calculate_optimal_iterations();
};

/**
 * @brief Shor 算法
 */
class ShorsAlgorithm {
public:
    ShorsAlgorithm(int N);
    
    // 因数分解
    std::pair<int, int> factorize();
    
private:
    int N_;
    
    // 量子部分：阶寻找
    QuantumCircuit build_order_finding_circuit(int a, int N);
    
    // 经典部分：继续分数算法
    std::vector<int> continued_fraction(double value, int N);
    
    // 检查因数
    bool check_factor(int a, int r, int N);
};

/**
 * @brief 量子傅里叶变换
 */
class QFT {
public:
    static QuantumCircuit build(size_t num_qubits);
    
    // 逆 QFT
    static QuantumCircuit build_inverse(size_t num_qubits);
    
private:
    static void build_qft_recursive(QuantumCircuit& circuit, size_t start, size_t n);
};

/**
 * @brief 量子相位估计
 */
class PhaseEstimation {
public:
    PhaseEstimation(size_t precision, size_t eigenstate_qubits);
    
    // 构建 QPE 电路
    QuantumCircuit build_circuit(const QuantumGate& unitary);
    
    // 估计相位
    double estimate_phase(size_t shots = 1000);
    
private:
    size_t precision_;
    size_t eigenstate_qubits_;
};

/**
 * @brief VQE (变分量子特征求解器)
 */
class VQE {
public:
    VQE(const std::vector<QuantumGate>& hamiltonian_terms);
    
    // 寻找基态
    std::pair<double, QuantumCircuit> find_ground_state(
        const std::function<QuantumCircuit(const std::vector<double>&)>& ansatz,
        const std::vector<double>& initial_params);
    
private:
    std::vector<QuantumGate> hamiltonian_terms_;
    
    // 计算期望值
    double compute_expectation(const QuantumCircuit& circuit);
};

/**
 * @brief 量子机器学习
 */
namespace QML {
    
/**
 * @brief 量子神经网络
 */
class QuantumNeuralNetwork {
public:
    QuantumNeuralNetwork(size_t num_qubits,
                        const std::vector<size_t>& layer_sizes);
    
    // 前向传播
    std::vector<Complex> forward(const std::vector<Complex>& input);
    
    // 训练
    void train(const std::vector<std::vector<Complex>>& inputs,
               const std::vector<std::vector<Complex>>& labels,
               size_t epochs);
    
    // 设置参数
    void set_parameters(const std::vector<double>& params);
    std::vector<double> get_parameters() const;
    
private:
    size_t num_qubits_;
    std::vector<size_t> layer_sizes_;
    std::vector<double> parameters_;
};

/**
 * @brief 量子支持向量机
 */
class QSVM {
public:
    QSVM(size_t num_qubits);
    
    // 训练
    void fit(const std::vector<std::vector<double>>& X,
             const std::vector<int>& y);
    
    // 预测
    int predict(const std::vector<double>& x);
    
private:
    size_t num_qubits_;
    std::vector<std::vector<double>> training_data_;
    std::vector<int> training_labels_;
};

} // namespace QML

} // namespace algorithms

/**
 * @brief 量子错误校正
 */
namespace ErrorCorrection {

/**
 * @brief 比特翻转码
 */
class BitFlipCode {
public:
    // 编码
    static QuantumCircuit encode(const QuantumState& state);
    
    // 解码
    static QuantumState decode(const QuantumCircuit& encoded);
    
    // 错误检测
    static bool detect_error(const QuantumState& state);
};

/**
 * @brief 相位翻转码
 */
class PhaseFlipCode {
public:
    // 编码
    static QuantumCircuit encode(const QuantumState& state);
    
    // 解码
    static QuantumState decode(const QuantumCircuit& encoded);
};

/**
 * @brief Shor 码
 */
class ShorCode {
public:
    // 编码（1 量子比特 -> 9 量子比特）
    static QuantumCircuit encode(const QuantumState& state);
    
    // 解码
    static QuantumState decode(const QuantumCircuit& encoded);
};

} // namespace ErrorCorrection

/**
 * @brief 量子编译器
 */
class QuantumCompiler {
public:
    // 优化电路
    static QuantumCircuit optimize(const QuantumCircuit& circuit);
    
    // 门分解
    static QuantumCircuit decompose_to_universal_set(const QuantumCircuit& circuit);
    
    // 拓扑排序
    static QuantumCircuit topological_sort(const QuantumCircuit& circuit);
    
    // 硬件感知映射
    static QuantumCircuit map_to_hardware(const QuantumCircuit& circuit,
                                          const std::vector<std::vector<int>>& connectivity);
    
    // 转换为 QASM
    static std::string to_qasm(const QuantumCircuit& circuit);
    
    // 转换为 Quil
    static std::string to_quil(const QuantumCircuit& circuit);
};

} // namespace quantum
} // namespace syclang

#endif // SYCLANG_QUANTUM_QUANTUM_RUNTIME_H
