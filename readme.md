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
### 7. 比較演算子
```
<, >, <=, =>, ==, != 
```
- 記号トークンの一般化 
### 8. 分割コンパイル
毎回プロジェクト全体のコードをコンパイルしていては時間の無駄
例えば、関数呼び出しに於いて、
- 名前が関数であるという識別子
- 関数の引数の情報
は必要だが
- 呼び出している関数の中身
は必要ない。つまり別ファイルにある関数を使うときに宣言だけは必要。

static宣言は外部ファイルで呼び出されないことを明示する
### 9. ローカル変数の導入(a ~ z)
変数: メモリアドレスに名前をつけたもの
ローカル変数: 関数"呼び出し"ごとに別々に置く => スタックを使う
現在の関数フレームの開始位置: x86ではRBPレジスタ
代入式の左辺値は変数単体(メモリアドレス)のみ
```
ロード命令　mov dst [src]
ストア命令 mov [dst] src
```
### 10. 文字列の名前を持つローカル変数
新たな変数が来たら連結リストに積んでいく
### 11. return文
```
statement = expr ";" | "return" expr ";"
```
### 12. 制御構文
- if
- while
- for
- 3項演算子
10.returnと同じようにstatementの構文に追加していく

### 注意
テストの値は0~255の範囲で
- シェルでは直前のコマンドの終了コードが$?という変数でアクセスできる
- Unixのプロセス終了コードは0〜255


### メモ
文字列を検索して、ファイル名と行番号を出力
```
grep -rn 文字列 src/*
```
### 参考
- 低レイヤを知りたい人のためのCコンパイラ作成入門
https://www.sigbus.info/compilerbook
