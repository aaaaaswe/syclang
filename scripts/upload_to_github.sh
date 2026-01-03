#!/bin/bash

# SysLang GitHub 自动上传脚本
# 自动提交并推送到 GitHub 仓库

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

# 检查是否在 Git 仓库中
if [ ! -d "$PROJECT_ROOT/.git" ]; then
    echo -e "${RED}错误: 不是 Git 仓库${NC}"
    echo "请先初始化 Git 仓库: git init"
    exit 1
fi

echo "================================================================"
echo "  SysLang GitHub 自动上传脚本"
echo "================================================================"
echo ""

# 检查是否有未提交的更改
if [ -n "$(git status --porcelain)" ]; then
    echo -e "${YELLOW}检测到未提交的更改${NC}"
    echo ""
    git status --short
    echo ""
    
    read -p "是否要查看详细更改? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        git diff
    fi
fi

echo ""
echo "================================================================"
echo "  Git 配置检查"
echo "================================================================"
echo ""

# 检查 Git 用户配置
GIT_NAME=$(git config user.name)
GIT_EMAIL=$(git config user.email)

if [ -z "$GIT_NAME" ]; then
    echo -e "${YELLOW}警告: 未设置 Git 用户名${NC}"
    read -p "请输入 Git 用户名: " git_name
    git config user.name "$git_name"
fi

if [ -z "$GIT_EMAIL" ]; then
    echo -e "${YELLOW}警告: 未设置 Git 邮箱${NC}"
    read -p "请输入 Git 邮箱: " git_email
    git config user.email "$git_email"
fi

echo -e "${GREEN}Git 用户配置:${NC}"
echo "  用户名: $(git config user.name)"
echo "  邮箱: $(git config user.email)"
echo ""

# 检查远程仓库
REMOTE_URL=$(git config --get remote.origin.url)

if [ -z "$REMOTE_URL" ]; then
    echo -e "${YELLOW}未配置远程仓库${NC}"
    echo ""
    echo "请输入 GitHub 仓库 URL (例如: https://github.com/username/syclang.git):"
    read remote_url
    
    git remote add origin "$remote_url"
    echo -e "${GREEN}远程仓库已添加: $remote_url${NC}"
else
    echo -e "${GREEN}远程仓库: $REMOTE_URL${NC}"
fi

echo ""
echo "================================================================"
echo "  准备提交"
echo "================================================================"
echo ""

# 添加所有文件
echo -e "${BLUE}添加所有文件到 Git...${NC}"
git add .

# 显示更改摘要
echo ""
echo "更改摘要:"
git status --short
echo ""

# 询问提交信息
DEFAULT_COMMIT_MSG="feat: 更新 SysLang v4.0，支持 11 种架构和完整 EFI 操作系统"

echo "默认提交信息:"
echo "  $DEFAULT_COMMIT_MSG"
echo ""
read -p "是否使用默认提交信息? (y/n) " -n 1 -r
echo

if [[ $REPLY =~ ^[Yy]$ ]]; then
    COMMIT_MSG="$DEFAULT_COMMIT_MSG"
else
    echo ""
    read -p "请输入提交信息: " commit_msg
    COMMIT_MSG="$commit_msg"
fi

echo ""
echo "================================================================"
echo "  提交更改"
echo "================================================================"
echo ""

# 创建提交
echo -e "${BLUE}创建 Git 提交...${NC}"
git commit -m "$COMMIT_MSG"

if [ $? -ne 0 ]; then
    echo -e "${RED}提交失败${NC}"
    echo "可能没有要提交的更改"
    exit 1
fi

echo -e "${GREEN}✓ 提交成功${NC}"
echo ""

echo "提交信息:"
git log -1 --pretty=format:"%h - %s (%cr)"
echo ""

# 检查当前分支
CURRENT_BRANCH=$(git branch --show-current)

echo "当前分支: $CURRENT_BRANCH"
echo ""

# 询问是否推送到 GitHub
read -p "是否推送到 GitHub? (y/n) " -n 1 -r
echo

if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "已取消推送"
    exit 0
fi

echo ""
echo "================================================================"
echo "  推送到 GitHub"
echo "================================================================"
echo ""

# 推送到远程仓库
echo -e "${BLUE}推送到 origin/$CURRENT_BRANCH...${NC}"
git push -u origin "$CURRENT_BRANCH"

if [ $? -ne 0 ]; then
    echo -e "${RED}推送失败${NC}"
    echo ""
    echo "可能的原因:"
    echo "  1. 网络连接问题"
    echo "  2. GitHub 仓库 URL 不正确"
    echo "  3. 认证失败（需要配置 SSH 密钥或个人访问令牌）"
    echo ""
    echo "解决方案:"
    echo "  1. 检查网络连接"
    echo "  2. 确认仓库 URL: git remote -v"
    echo "  3. 配置 Git 凭据或使用 SSH"
    exit 1
fi

echo -e "${GREEN}✓ 推送成功！${NC}"
echo ""

# 显示仓库链接
echo "================================================================"
echo "  上传完成！"
echo "================================================================"
echo ""
echo "你的代码已成功推送到 GitHub"
echo ""
echo "仓库链接: $REMOTE_URL"
echo ""
echo "Actions: https://github.com/$(echo $REMOTE_URL | sed 's|https://github.com/||' | sed 's|.git||')/actions"
echo "Issues: https://github.com/$(echo $REMOTE_URL | sed 's|https://github.com/||' | sed 's|.git||')/issues"
echo ""

# 生成 Release 说明
echo "================================================================"
echo "  可用的后续操作"
echo "================================================================"
echo ""
echo "1. 在 GitHub 上创建 Release:"
echo "   gh release create v4.0.0 \\"
echo "     --title 'SysLang v4.0 - 量子计算时代' \\"
echo "     --notes '支持 11 种架构和 15 种文件系统的完整 EFI 操作系统'"
echo ""
echo "2. 启用 GitHub Actions:"
echo "   访问仓库的 Actions 标签页启用 CI/CD"
echo ""
echo "3. 创建 Pull Request (如果有分支):"
echo "   gh pr create --title 'Update v4.0' --body 'Features and fixes'"
echo ""
echo "4. 查看仓库统计:"
echo "   gh repo view"
echo ""

exit 0
