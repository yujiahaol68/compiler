# 编译原理实验

系统环境：Mac OS

工具：CMake

## 词法分析

commit 9d2f0348eddcf2725f55e8c81bbd37b794d67902

```bash
./cmake-build-debug/compiler < lexer_test.txt
```

## 语法分析

```bash
./cmake-build-debug/compiler < ./semantics_test.txt
```

## 语义分析

```bash
./cmake-build-debug/compiler < ./stmt_test.txt
```
