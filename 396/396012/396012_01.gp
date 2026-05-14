G_kr(k, r, N) = {
    my(x = 'x + O('x^N));
    \\ P(j) を計算する内部処理
    my(get_P(j) = if(j <= 0, 1, 1 / prod(i = 1, j, 1 - x^i)));
    
    \\ 第1の表現（累積和）を計算
    sum(m = 1, k, x^(m*(r-1) + k) * get_P(k-m));
}

res = G_kr(5, 3, 100);
for(n=0, 90, print1(polcoef(res, n), ", "));


