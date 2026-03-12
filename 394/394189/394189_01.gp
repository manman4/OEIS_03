default(parisize, 1200000000)

\\ (distinct parts) < (min multiplicity) を計算する関数
part_min_mult(n) = {
    my(q = 'q + O('q^(n+1)));
    my(total_gf = 0); 
    
    for(k = 1, n,
        \\ 異なるパーツが k 個で、各重複数が k 以上の最小の n は k^2(k+1)/2
        if(k^2*(k+1)/2 > n, break);
        
        \\ z を変数として、積を計算
        \\ z の k 次の係数が「異なるパーツがちょうど k 個」のケースに相当
        my(P = prod(j = 1, n\k, 1 + 'z * q^((k+1)*j) / (1 - q^j)));
        
        \\ z^k の係数（q のべき級数）を足し合わせる
        total_gf += polcoeff(P, k, 'z);
    );
    
    return(total_gf);
}

M=1000;
res = part_min_mult(M); 
for(n=1, M, write("b394189.txt", n, " ", polcoef(res, n)));
