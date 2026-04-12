{
  my(N = 11);
  my(v = vector(N), x = 'x);

  for(n = 1, N,
    my(An_prev = sum(i=1, n-1, v[i]*x^i));
    my(Q_prev = n^2 * An_prev + O(x^(n+1)));
    v[n] = -polcoeff(exp(Q_prev)*(1-n^3*x + O(x^(n+1))), n) / n^2;
  );

  \\ 表の各行を出力
  for(n = 1, N,
    my(An = sum(i=1, N, v[i]*x^i));
    my(En = exp(n^2 * An + O(x^(N+1)))*(1-n^3*x + O(x^(N+1))));
    
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
a = [1, 6, 216, 18016, 2718575, 667151244, 249904389518, 136335045655680, 104258627494173747, 108236370325030253850];
\\ exp( n^2*A(x) )*(1-n^3*x)
print(Vec( exp( 1^2*sum(i=1, #a, a[i]*x^i) )*(1 - 1^3*x + O(x^10))));
print(Vec( exp( 2^2*sum(i=1, #a, a[i]*x^i) )*(1 - 2^3*x + O(x^10))));
print(Vec( exp( 3^2*sum(i=1, #a, a[i]*x^i) )*(1 - 3^3*x + O(x^10))));
print(Vec( exp( 4^2*sum(i=1, #a, a[i]*x^i) )*(1 - 4^3*x + O(x^10))));
print(Vec( exp( 5^2*sum(i=1, #a, a[i]*x^i) )*(1 - 5^3*x + O(x^10))));
print(Vec( exp( 6^2*sum(i=1, #a, a[i]*x^i) )*(1 - 6^3*x + O(x^10))));
print(Vec( exp( 7^2*sum(i=1, #a, a[i]*x^i) )*(1 - 7^3*x + O(x^10))));
print(Vec( exp( 8^2*sum(i=1, #a, a[i]*x^i) )*(1 - 8^3*x + O(x^10))));
print(Vec( exp( 9^2*sum(i=1, #a, a[i]*x^i) )*(1 - 9^3*x + O(x^10))));



