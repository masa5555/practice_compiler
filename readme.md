# コンパイラ学習
### 1. 整数１つ
Makefile, test.sh, .gitignoreを作成
### 2. 加減算への対応
### 3. トークナイザの導入
### 4. エラー箇所の列を指摘するように改良
### 5. 優先順位に従って演算できるように
```
expr = mul ('+' mul | '-' mul)*
mul = primary ('*' primary | "/" primary)*
primary = num | '(' expr ')'
```
### 参考
- 低レイヤを知りたい人のためのCコンパイラ作成入門
https://www.sigbus.info/compilerbook