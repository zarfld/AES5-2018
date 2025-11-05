# Contributing to AES5-2018

Thank you for your interest in contributing to the AES5-2018 library! This document provides guidelines for contributing to the project.

## Code of Conduct

This project adheres to a code of conduct that all contributors are expected to follow. Please be respectful and constructive in your interactions with other contributors.

## How to Contribute

### Reporting Bugs

If you find a bug, please open an issue with:
- A clear, descriptive title
- Steps to reproduce the issue
- Expected behavior
- Actual behavior
- Your environment (Node.js version, OS, etc.)
- Code samples if applicable

### Suggesting Enhancements

We welcome suggestions for enhancements! Please open an issue with:
- A clear, descriptive title
- Detailed description of the proposed feature
- Why this enhancement would be useful
- Example usage if applicable

### Pull Requests

1. Fork the repository
2. Create a new branch for your feature or fix
3. Make your changes
4. Add or update tests as needed
5. Ensure all tests pass: `npm test`
6. Update documentation if needed
7. Submit a pull request

#### Pull Request Guidelines

- Follow the existing code style
- Include tests for new functionality
- Update README.md if adding new features
- Keep commits atomic and well-described
- Reference any related issues

## Development Setup

```bash
# Clone the repository
git clone https://github.com/zarfld/AES5-2018.git
cd AES5-2018

# Run tests
npm test

# Run examples
node examples/basic-usage.js
node examples/advanced-usage.js
```

## Testing

All contributions should include appropriate tests. We use Node.js's built-in test runner.

```bash
npm test
```

Tests are located in the `test/` directory and should:
- Cover new functionality
- Test edge cases
- Be clear and well-documented
- Follow existing test patterns

## Code Style

- Use ES6+ features
- Follow JSDoc conventions for documentation
- Use meaningful variable and function names
- Keep functions focused and single-purpose
- Add comments for complex logic

## Documentation

- Update README.md for user-facing changes
- Update JSDoc comments for API changes
- Update examples if relevant
- Update CHANGELOG.md

## Versioning

This project follows [Semantic Versioning](https://semver.org/):
- MAJOR version for incompatible API changes
- MINOR version for backwards-compatible functionality additions
- PATCH version for backwards-compatible bug fixes

## Standards Compliance

All contributions must maintain compliance with the AES5-2018 standard. When adding features:
- Reference the relevant section of the standard
- Ensure accuracy of sampling rate information
- Maintain compatibility with AES3, AES11, AES31, AES67, and MILAN

## License

By contributing to this project, you agree that your contributions will be licensed under the MIT License.

## Questions?

If you have questions about contributing, feel free to open an issue for discussion.

## Recognition

Contributors will be recognized in the project's documentation. Thank you for helping improve AES5-2018!
