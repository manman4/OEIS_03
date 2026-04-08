{
  my(N = 11); \\ 出力する行列のサイズ
  my(v = vector(N), x = 'x);

  \\ 1. まず a(n) を N 項求める
  for(n = 1, N,
    my(An_prev = sum(i=1, n-1, v[i]*x^i));
    my(Q_prev = (n*x)/(1 - n*x) - n*An_prev + O(x^(n+1)));
    v[n] = polcoeff(exp(Q_prev), n) / n;
  );

  \\ 2. 各 n について exp(...) の係数を計算し、表形式で出力
  print("n \\ k |  0   1   2   3   4   5   6");
  print("------+--------------------------------");
  for(n = 1, N,
    my(An = sum(i=1, N, v[i]*x^i));
    my(En = exp((n*x)/(1 - n*x) - n*An + O(x^(N+1))));
    
    printf("%2d    |", n);
    for(k = 0, N,
      printf("%3d ", polcoeff(En, k));
    );
    print();
  );
  
  print("\nDerived a(n): ", v);
}