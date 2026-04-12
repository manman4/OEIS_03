{
  my(N = 11);
  my(v = vector(N), x = 'x);

  for(n = 1, N,
    my(An_prev = sum(i=1, n-1, v[i]*x^i));
    my(Q_prev = n * An_prev + O(x^(n+1)));
    v[n] = -polcoeff(exp(Q_prev)*(1-n^3*x + O(x^(n+1))), n) / n;
  );

  \\ 表の各行を出力
  for(n = 1, N,
    my(An = sum(i=1, N, v[i]*x^i));
    my(En = exp(n * An + O(x^(N+1)))*(1-n^3*x + O(x^(N+1))));
    
    \\ 【修正箇所】 printf("%d, ", ...) を print1(..., ", ") に変更
    for(k = 0, N, print1(polcoeff(En, k), ", "));
    print();
  );

  \\ 最後に a(n) を出力
  print("--- a(n) ---");
  
  \\ 【修正箇所】 ここも念のため print1 に統一
  for(n = 1, N, print1(v[n], ", "));
  print();
}

\\ 確認用
a = [1, 7, 207, 14226, 1852800, 409408077, 142286748933, 73448832515952, 53835885818473473, 54041298732304775000, 72129250579997923194091];
\\ exp( n*A(x) )*(1-n^3*x)
print(Vec( exp( 1*sum(i=1, #a, a[i]*x^i) )*(1 - 1^3*x + O(x^10))));
print(Vec( exp( 2*sum(i=1, #a, a[i]*x^i) )*(1 - 2^3*x + O(x^10))));
print(Vec( exp( 3*sum(i=1, #a, a[i]*x^i) )*(1 - 3^3*x + O(x^10))));
print(Vec( exp( 4*sum(i=1, #a, a[i]*x^i) )*(1 - 4^3*x + O(x^10))));
print(Vec( exp( 5*sum(i=1, #a, a[i]*x^i) )*(1 - 5^3*x + O(x^10))));
print(Vec( exp( 6*sum(i=1, #a, a[i]*x^i) )*(1 - 6^3*x + O(x^10))));
print(Vec( exp( 7*sum(i=1, #a, a[i]*x^i) )*(1 - 7^3*x + O(x^10))));
print(Vec( exp( 8*sum(i=1, #a, a[i]*x^i) )*(1 - 8^3*x + O(x^10))));
print(Vec( exp( 9*sum(i=1, #a, a[i]*x^i) )*(1 - 9^3*x + O(x^10))));



