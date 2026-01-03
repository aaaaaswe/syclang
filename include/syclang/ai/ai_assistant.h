/**
 * @file ai_assistant.h
 * @brief AI 辅助编程工具 - SysLang v3.0
 * 
 * 提供 AI 驱动的代码分析、优化建议和错误修复
 * 
 * SysLang v3.0 新增功能：AI 辅助编程
 */

#ifndef SYCLANG_AI_AI_ASSISTANT_H
#define SYCLANG_AI_AI_ASSISTANT_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

namespace syclang {
namespace ai {

/**
 * @brief 分析类型
 */
enum class AnalysisType {
    PERFORMANCE_OPTIMIZATION,
    CODE_QUALITY,
    SECURITY,
    BUG_DETECTION,
    MEMORY_LEAK,
    DEADLOCK,
    RACE_CONDITION,
    ALGORITHM_COMPLEXITY
};

/**
 * @brief 分析级别
 */
enum class AnalysisLevel {
    QUICK,       // 快速分析
    STANDARD,    // 标准分析
    DEEP,        // 深度分析
    THOROUGH     // 彻底分析
};

/**
 * @brief 代码问题
 */
struct CodeIssue {
    enum class Severity {
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };
    
    Severity severity;
    std::string message;
    std::string file;
    int line;
    int column;
    std::string suggested_fix;
    
    CodeIssue(Severity s, const std::string& msg, const std::string& f, int l, int c,
              const std::string& fix = "")
        : severity(s), message(msg), file(f), line(l), column(c), suggested_fix(fix) {}
};

/**
 * @brief 优化建议
 */
struct OptimizationSuggestion {
    enum class Impact {
        LOW,
        MEDIUM,
        HIGH,
        VERY_HIGH
    };
    
    Impact impact;
    std::string description;
    std::string before_code;
    std::string after_code;
    int estimated_speedup_percent;
    std::string explanation;
    
    OptimizationSuggestion(Impact i, const std::string& desc, const std::string& before,
                           const std::string& after, int speedup, const std::string& expl)
        : impact(i), description(desc), before_code(before), after_code(after),
          estimated_speedup_percent(speedup), explanation(expl) {}
};

/**
 * @brief AI 辅助编程助手
 */
class AIAssistant {
public:
    AIAssistant();
    virtual ~AIAssistant();
    
    // 代码分析
    std::vector<CodeIssue> analyze_code(const std::string& code,
                                       AnalysisType type,
                                       AnalysisLevel level = AnalysisLevel::STANDARD);
    
    std::vector<OptimizationSuggestion> suggest_optimizations(const std::string& code,
                                                               AnalysisLevel level = AnalysisLevel::STANDARD);
    
    // 错误修复
    std::string auto_fix_errors(const std::string& code,
                                const std::vector<CodeIssue>& issues);
    
    // 代码补全
    std::vector<std::string> code_completion(const std::string& code_prefix,
                                             int cursor_pos,
                                             int max_suggestions = 10);
    
    // 智能重构
    std::string refactor_code(const std::string& code,
                             const std::string& refactor_type);
    
    // 文档生成
    std::string generate_documentation(const std::string& code,
                                       const std::string& format = "markdown");
    
    // 单元测试生成
    std::string generate_unit_tests(const std::string& code);
    
    // 代码解释
    std::string explain_code(const std::string& code);
    
    // 设置配置
    void set_model(const std::string& model_name);
    void set_temperature(float temperature);
    void set_max_tokens(int max_tokens);
    
private:
    // 调用 LLM API
    std::string call_llm(const std::string& prompt);
    
    // 构建 Prompt
    std::string build_analysis_prompt(const std::string& code,
                                      AnalysisType type,
                                      AnalysisLevel level);
    
    std::string build_optimization_prompt(const std::string& code,
                                          AnalysisLevel level);
    
    std::string build_fix_prompt(const std::string& code,
                                const std::vector<CodeIssue>& issues);
    
    std::string build_completion_prompt(const std::string& code_prefix,
                                        int cursor_pos);
    
    // 解析 LLM 响应
    std::vector<CodeIssue> parse_issues(const std::string& response);
    std::vector<OptimizationSuggestion> parse_optimizations(const std::string& response);
    
    // 配置
    std::string model_;
    float temperature_;
    int max_tokens_;
    std::string api_key_;
    std::string api_base_;
};

/**
 * @brief 实时代码分析器
 */
class LiveCodeAnalyzer {
public:
    LiveCodeAnalyzer();
    ~LiveCodeAnalyzer();
    
    // 开始实时分析
    void start_analysis(const std::string& file_path);
    
    // 停止分析
    void stop_analysis();
    
    // 获取当前问题
    std::vector<CodeIssue> get_current_issues();
    
    // 设置回调函数
    void set_issue_callback(std::function<void(const CodeIssue&)> callback);
    
    // 配置分析选项
    void enable_realtime_error_detection(bool enable);
    void set_analysis_interval(int milliseconds);
    
private:
    void analysis_loop();
    
    std::string file_path_;
    bool running_;
    std::thread analysis_thread_;
    std::vector<CodeIssue> current_issues_;
    std::function<void(const CodeIssue&)> issue_callback_;
    bool realtime_error_detection_;
    int analysis_interval_ms_;
    std::mutex issues_mutex_;
};

/**
 * @brief AI 驱动的测试生成器
 */
class AITestGenerator {
public:
    // 生成测试用例
    std::string generate_test_cases(const std::string& function_code,
                                   int num_cases = 10);
    
    // 生成边界测试
    std::string generate_boundary_tests(const std::string& function_code);
    
    // 生成模糊测试
    std::string generate_fuzz_tests(const std::string& function_code);
    
    // 生成性能测试
    std::string generate_performance_tests(const std::string& function_code);
    
    // 生成属性测试
    std::string generate_property_tests(const std::string& function_code);
    
private:
    std::vector<std::string> extract_function_signatures(const std::string& code);
    std::string generate_test_case_for_signature(const std::string& signature);
};

/**
 * @brief AI 代码审查助手
 */
class AICodeReviewer {
public:
    // 审查代码
    std::vector<CodeIssue> review_code(const std::string& code);
    
    // 代码风格检查
    std::vector<CodeIssue> check_style(const std::string& code);
    
    // 最佳实践检查
    std::vector<CodeIssue> check_best_practices(const std::string& code);
    
    // 安全审计
    std::vector<CodeIssue> security_audit(const std::string& code);
    
    // 可维护性评估
    int calculate_maintainability_score(const std::string& code);
    
    // 技术债务分析
    std::vector<std::string> analyze_technical_debt(const std::string& code);
    
private:
    std::vector<CodeIssue> analyze_code_quality(const std::string& code);
    std::vector<CodeIssue> analyze_security_vulnerabilities(const std::string& code);
};

/**
 * @brief AI 智能重构工具
 */
class AIRefactoringTool {
public:
    // 重命名变量/函数
    std::string rename_symbol(const std::string& code,
                             const std::string& old_name,
                             const std::string& new_name);
    
    // 提取函数
    std::string extract_function(const std::string& code,
                                 int start_line,
                                 int end_line,
                                 const std::string& function_name);
    
    // 内联函数
    std::string inline_function(const std::string& code,
                               const std::string& function_name);
    
    // 提取变量
    std::string extract_variable(const std::string& code,
                                 int start_pos,
                                 int end_pos,
                                 const std::string& variable_name);
    
    // 引入参数对象
    std::string introduce_parameter_object(const std::string& code,
                                           const std::vector<std::string>& params);
    
    // 简化条件表达式
    std::string simplify_conditionals(const std::string& code);
    
    // 消除重复代码
    std::string eliminate_duplicates(const std::string& code);
    
    // 转换为现代语法
    std::string modernize_code(const std::string& code);
    
private:
    std::string apply_refactoring_with_ai(const std::string& code,
                                         const std::string& refactoring_description);
};

} // namespace ai
} // namespace syclang

#endif // SYCLANG_AI_AI_ASSISTANT_H
