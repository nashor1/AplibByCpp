# aplib

## 叙述

这是一个aplib压缩解压的cpp实现

## 环境需要

编译环境为vs2022 x86/x64

## 目的

旨在为开发人员传输数据过程中提供一种压缩解压的办法实现，并非采用现代的zip压缩方式，Aplib中使用的实际算法是远程的，但它建立在鱼LZ78非常不同的基础上，并且与LZ77有更多的关系，该格式的一个不太常见的功能包括使用rep-offsets的可能性，即允许重用偏移量的命令。这个想法很可能是从LZX中借鉴过来的，这个特性使得Aplib在压缩方面比许多其他固定格式的LZ77/LZSS压缩器好得多

## 引用

可参考aplib开源官网：[Ibsen Software  [ home \]](https://www.ibsensoftware.com/)
