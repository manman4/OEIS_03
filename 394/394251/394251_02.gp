\\ ニュートンの恒等式を用いて、異なる k 種類のパーツからなり、
\\ 各パーツの重複数上限が max_m である分割の母関数（z^k の係数）を計算する
\\ G.f. [z^k] Product_{j>=1} (1 + z * (q^j + q^(2j) + ... + q^(max_m*j)))
get_ek_limit(k, max_m, n) = {
    my(S = vector(k));
    my(q_prec = 'q + O('q^(n+1)));
    
    \\ べき乗和 S_r = sum_{j>=1} (X_j)^r を計算
    \\ X_j = q^j * (1 - q^(max_m * j)) / (1 - q^j)
    for(r = 1, k,
        \\ j*r > n となる項は寄与しないため n\r で打ち切り
        S[r] = sum(j = 1, n\r, (q_prec^j * (1 - q_prec^(max_m * j)) / (1 - q_prec^j))^r);
    );
    
    \\ 基本対称式 e_k を計算 (Newton's sums)
    my(e = vector(k+1));
    e[1] = 1; \\ e_0 = 1
    for(i = 1, k,
        e[i+1] = sum(j = 1, i, (-1)^(j-1) * e[i-j+1] * S[j]) / i;
    );
    return(e[k+1]);
}

\\ A394251: (最大重複数) <= 3 * (異なるパーツの種類数)
fast_A394251(M) = {
    my(total_gf = 1); \\ i=0 のケース
    
    for(i = 1, M,
        \\ 異なるパーツが i 個ある最小の和は i(i+1)/2
        if(i*(i+1)/2 > M, break);
        
        \\ 数式の条件: 種類数 i に対して 最大重複数 = 3*i
        \\ z^i の係数を計算
        total_gf += get_ek_limit(i, 3*i, M);
    );
    return(total_gf);
}

M = 1000;
res = fast_A394251(M);
\\ 結果の保存
for(n=0, M, write("b394251_fast.txt", n, " ", polcoef(res, n)));