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
    \\ 【修正箇所】 printf("%d, ", ...) を print1(..., ", ") に変更
    for(k = 0, N, print1(polcoeff(En, k), ", "));
    print();
  );

  \\ 最後に a(n) を出力
  print("--- a(n) ---");
  for(n = 1, N, print1(v[n], ", "));
  print();
}

\\ 確認用
a = [1, 4, 81, 4384, 447745, 73207683, 17470684441, 5729487641704, 2473348488736401, 1360426574429100415, 929288672122718278921]
\\ exp( n*x/(1-n^2*x) - n*A(x) )を出力
print(Vec(exp(1 * x/(1 - 1^2*x) - 1*sum(i=1, #a, a[i]*x^i)) + O(x^10)));
print(Vec(exp(2 * x/(1 - 2^2*x) - 2*sum(i=1, #a, a[i]*x^i)) + O(x^10)));
print(Vec(exp(3 * x/(1 - 3^2*x) - 3*sum(i=1, #a, a[i]*x^i)) + O(x^10)));
print(Vec(exp(4 * x/(1 - 4^2*x) - 4*sum(i=1, #a, a[i]*x^i)) + O(x^10)));
print(Vec(exp(5 * x/(1 - 5^2*x) - 5*sum(i=1, #a, a[i]*x^i)) + O(x^10)));
print(Vec(exp(6 * x/(1 - 6^2*x) - 6*sum(i=1, #a, a[i]*x^i)) + O(x^10)));