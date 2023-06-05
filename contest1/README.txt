1) solve.c のコンパイル

$ gcc solve.c -o solve.out -lm

2) 実行

$ ./solve.out tspファイル名 

で実行すると tour1.dat 及び tour2.dat というファイルができる. 
これらのファイルには, それぞれ最近近傍法による巡回路, 及び, 
それに対して2-opt法を実行した結果が書かれている. 
(注: ただし配布したプログラムの2-optは何もしないので, 
tour1.dat = tour2.dat となっている. 
)

例)

$ ./solve.out in_data/instance10-1.tsp

3) 図で確認 (先週の課題2のプログラムを使う)
  キットの中の draw.c をコンパイル
$  gcc draw.c -o draw.out -lm -lgd
して事前に実行ファイル draw.out を生成しておく.

$ ./draw.out tspファイル名 < 巡回路ファイル > 画像ファイル名

例)

$ ./draw.out in_data/instance10-1.tsp < tour1.dat > fig1.png
$ ./draw.out in_data/instance10-1.tsp < tour2.dat > fig2.png
$ eog fig1.png
$ eog fig2.png


