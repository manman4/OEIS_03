{
  my(N = 215);
  my(v = vector(N), x = 'x);

  for(n = 1, N,
    my(An_prev = sum(i=1, n-1, v[i]*x^i));
    my(Q_prev = n * (x/(1 - n^2*x) - An_prev) + O(x^(n+1)));
    v[n] = polcoeff(exp(Q_prev), n) / n;
  );

  
  \\ 【修正箇所】 ここも念のため print1 に統一
  for(n = 1, N,
    write("b394969_1.txt", n, " ", v[n]);
  );
}
