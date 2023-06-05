0. 説明

check_validity は巡回路ファイルが順序制約を守っている巡回路を表現
しているファイルであるかどうかをチェックするプログラム. 

1. コンパイル

$ gcc check_validity.c -o check_validity.out -lm

2. 実行の仕方

$ ./check_validity.out <dat filename> <tsp filename>

3. 実行例

$ ./solve.out in_data2/d198.tsp
34965.6
$ ./check_validity.out tour1.dat in_data2/d198.tsp
Valid dat file. 34965.607960

3) 図で確認 
キットの中の draw.c をコンパイル
$  gcc draw.c -o draw.out -lm -lgd
して事前に実行ファイル draw.out を生成しておく.

$ ./draw.out tspファイル名 < 巡回路ファイル > 画像ファイル名

実行例)

$ ./draw.out in_data/instance10-1.tsp < tour.dat > fig.png
$ eog fig.png



