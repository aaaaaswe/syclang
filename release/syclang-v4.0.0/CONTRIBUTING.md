# Contributing to SysLang

Thank you for your interest in contributing to SysLang! This document provides guidelines for contributing to the project.

## How to Contribute

### Reporting Bugs

Before creating bug reports, please check the existing issues to avoid duplicates. When creating a bug report, please include:

* A clear and descriptive title
* Steps to reproduce the issue
* Expected behavior
* Actual behavior
* Environment details (OS, compiler, architecture)
* Any relevant code snippets or logs

### Suggesting Enhancements

Enhancement suggestions are welcome! When suggesting an enhancement, please:

* Use a clear and descriptive title
* Provide a detailed description of the proposed enhancement
* Explain why this enhancement would be useful
* Provide examples if applicable

### Pull Requests

1. Fork the repository
2. Create a new branch for your feature/fix
3. Make your changes
4. Add tests if applicable
5. Ensure all tests pass
6. Update documentation if needed
7. Submit a pull request with a clear description

## Coding Standards

### Code Style

* Follow C++20 coding standards
* Use meaningful variable and function names
* Add comments for complex logic
* Keep functions focused and concise
* Use RAII for resource management

### Code Organization

* Header files go in `include/syclang/`
* Implementation files go in `src/`
* Tests go in `tests/`

## Development Workflow

1. Clone your fork: `git clone https://github.com/your-username/syclang.git`
2. Create a branch: `git checkout -b feature/my-feature`
3. Make changes and commit: `git commit -m "Add my feature"`
4. Push to your fork: `git push origin feature/my-feature`
5. Create a pull request

## Building

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Testing

```bash
cd build
ctest
```

## Documentation

When adding new features, please update the relevant documentation:
* README.md for user-facing changes
* Code comments for implementation details
* Examples directory for usage examples

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

## Questions?

Feel free to open an issue for any questions!
