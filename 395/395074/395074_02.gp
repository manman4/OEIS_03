{
  my(N = 215);
  my(v = vector(N));
  my(x = 'x);

  for(n = 1, N,
    \\ 1. 既知の係数からなる A(x) mod x^n
    my(An_prev = sum(i=1, n-1, v[i]*x^i + O(x^(n+1))));
    
    \\ 2. 条件式の exp 内部を正しく構成
    \\ exp( -n*A(x) - 1/n*log(1 - n^2*x) ) = exp(-n*A(x)) * (1 - n^2*x)^-(1/n)
    my(Q = -n * An_prev - 1/n*log(1 - n^2*x + O(x^(n+1))));
    
    \\ 3. n次の係数から a(n) を分離して決定
    \\ [x^n] exp(Q - n*a(n)*x^n) = [x^n] exp(Q)*(1 - n*a(n)*x^n) = [x^n] exp(Q) - n*a(n) = 0
    v[n] = polcoeff(exp(Q), n) / n;

    \\ ファイルへの書き出し
    write("b395074_1.txt", n, " ", v[n]);
  );
}
