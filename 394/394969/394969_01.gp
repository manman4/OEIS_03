{
  my(N = 11);
  my(v = vector(N), x = 'x);

  for(n = 1, N,
    my(An_prev = sum(i=1, n-1, v[i]*x^i));
    my(Q_prev = n * (x/(1 - n^2*x) - An_prev) + O(x^(n+1)));
    v[n] = polcoeff(exp(Q_prev), n) / n;
  );

  \\ 表の各行を出力
  for(n = 1, N,
    my(An = sum(i=1, N, v[i]*x^i));
    my(En = exp(n * (x/(1 - n^2*x) - An) + O(x^(N+1))));
    for(k = 0, N, printf("%d, ", polcoeff(En, k)));
    print();
  );

  \\ 最後に a(n) を出力
  print("--- a(n) ---");
  for(n = 1, N, printf("%d, ", v[n]));
  print();
}