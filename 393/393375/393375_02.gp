default(parisize, 1200000000)

part_min_mult_optimized(n) = {
    my(q = 'q + O('q^(n+1)));
    my(total_gf = 1);  \\ j=0 のケース
    
    for(j = 1, n,
        \\ 最小の n の境界（3j <= mult の場合）
        \\ 各パーツ k_1, k_2, ..., k_j (>=1) が 3j 回以上現れる最小の和
        \\ 3j * (1 + 2 + ... + j) = 3j^2(j+1)/2
        my(min_n = 3 * j^2 * (j+1) / 2);
        if(min_n > n, break);
        
        \\ 積の範囲を k <= n \ (3*j) に制限して高速化
        \\ また、z^j の係数を直接計算するために、z を含む積ではなく
        \\ j 個の異なるパーツを選ぶ組み合わせの重みを計算する
        \\ ここでは polcoef を使う元の構造を維持しつつ k の範囲を最適化
        my(P = prod(k = 1, n \ (3*j), 1 + 'z * q^(3*j*k) / (1 - q^k)));
        total_gf += polcoef(P, j, 'z);
    );
    return(total_gf);
}

M=1000;
res = part_min_mult_optimized(M);
\\ for(n=0, M, print1(polcoef(res, n), ", "));
for(n=0, M, write("b393375_1.txt", n, " ", polcoef(res, n)));
