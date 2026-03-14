default(parisize, 1200000000)

part_min_mult_2j(n) = {
    my(q = 'q + O('q^(n+1)));
    my(total_gf = 1); \\ j=0 (空の分割) を 1 とカウント
    
    for(j = 1, n,
        \\ 最小の n: 2j * (1 + 2 + ... + j) = j^2(j+1)
        my(min_n = j^2 * (j+1));
        if(min_n > n, break);
        
        \\ z の j 次の係数を抽出。k の上限を絞って高速化
        \\ G.f.: [z^j] Product_{k>=1} (1 + z * q^(2*j*k) / (1-q^k))
        my(P = prod(k = 1, n \ (2*j), 1 + 'z * q^(2*j*k) / (1 - q^k)));
        total_gf += polcoef(P, j, 'z);
    );
    
    return(total_gf);
}

M=1000;
res = part_min_mult_2j(M);
\\ for(n=0, M, print1(polcoef(res, n), ", "));
for(n=0, M, write("b393298_1.txt", n, " ", polcoef(res, n)));
