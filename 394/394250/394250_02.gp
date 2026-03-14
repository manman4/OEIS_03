\\ ニュートンの恒等式を用いて、異なる k 種類のパーツからなり、
\\ 各パーツの重複数上限が 2k である分割の母関数（z^k の係数）を計算
get_ek_limit_2k(k, n) = {
    my(S = vector(k));
    my(q_prec = 'q + O('q^(n+1)));
    my(m = 2 * k);  \\ 重複数の上限は 2*k
    
    \\ べき乗和 S_r = sum_{j>=1} (X_j)^r を計算
    \\ ここで X_j = q^j + q^{2j} + ... + q^{mj} = q^j(1-q^{mj})/(1-q^j)
    for(r = 1, k,
        \\ j*r > n となる項は計算不要
        S[r] = sum(j = 1, n\r, (q_prec^j * (1 - q_prec^(m*j)) / (1 - q_prec^j))^r);
    );
    
    \\ 基本対称式 e_k (z^k の係数) をニュートンの恒等式で導出
    my(e = vector(k+1));
    e[1] = 1; \\ e_0 = 1
    for(i = 1, k,
        e[i+1] = sum(j = 1, i, (-1)^(j-1) * e[i-j+1] * S[j]) / i;
    );
    return(e[k+1]);
}

fast_A394250_final(M) = {
    my(total_gf = 1); \\ i=0 のケース (空の分割)
    my(q = 'q + O('q^(M+1)));
    
    for(i = 1, M,
        \\ i 個の異なるパーツの最小和は i(i+1)/2。
        \\ 各パーツが少なくとも1つ選ばれるため、この条件で break 可能。
        if(i*(i+1)/2 > M, break);
        
        \\ z^i の係数を計算して加算
        total_gf += get_ek_limit_2k(i, M);
    );
    return(total_gf);
}

\\ 実行と保存
M = 1000;
res = fast_A394250_final(M);
for(n=0, M, write("b394250_fast.txt", n, " ", polcoef(res, n)));