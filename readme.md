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

### 6. 単項+-に対応
```
expr    = mul ("+" mul | "-" mul)*
mul     = unary ("*" unary | "/" unary)*
unary   = ("+" | "-")? primary
primary = num | "(" expr ")"
```
### 注意
テストの値は0~255の範囲で
- シェルでは直前のコマンドの終了コードが$?という変数でアクセスできる
- Unixのプロセス終了コードは0〜255

### 参考
- 低レイヤを知りたい人のためのCコンパイラ作成入門
https://www.sigbus.info/compilerbook