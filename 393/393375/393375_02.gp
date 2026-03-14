default(parisize, 1200000000)

\\ (distinct parts) <= (min multiplicity) を計算する関数
part_min_mult(n) = {
    my(q = 'q + O('q^(n+1)));
    my(total_gf = 1); 
    
    for(j = 1, n,
        \\ 異なるパーツが j 個で、各重複数が j 以上の最小の n は j^2(j+1)/2
        if(j^2*(j+1)/2 > n, break);
        
        \\ z を変数として、積を計算
        \\ z の j 次の係数が「異なるパーツがちょうど j 個」のケースに相当
        \\ G.f.: Sum_{j>=0} [z^j] Product_{k>=1} (1 + z*q^(3*j*k)/(1-q^k)).
        my(P = prod(k = 1, n, 1 + 'z * q^(3*j*k) / (1 - q^k)));
        
        \\ z^j の係数（q のべき級数）を足し合わせる
        total_gf += polcoef(P, j, 'z);
    );
    
    return(total_gf);
}

M=1000;
res = part_min_mult(M);
\\ for(n=0, M, print1(polcoef(res, n), ", "));
for(n=0, M, write("b393375_1.txt", n, " ", polcoef(res, n)));
