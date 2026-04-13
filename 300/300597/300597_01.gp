\\ G.f. A(x) satisfies [x^n] exp(n^4*A(x)) * (1 - n^4*x) = 0, for n >= 1.
{
  my(N = 9);
  my(v = vector(N), x = 'x);

  for(n = 1, N,
    my(An_prev = sum(i=1, n-1, v[i]*x^i));
    my(Q_prev = n^4 * An_prev + O(x^(n+1)));
    v[n] = -polcoeff(exp(Q_prev)*(1-n^4*x + O(x^(n+1))), n) / n^4;
  );

  \\ 表の各行を出力
  for(n = 1, N,
    my(An = sum(i=1, N, v[i]*x^i));
    my(En = exp(n^4 * An + O(x^(N+1)))*(1-n^4*x + O(x^(N+1))));
    
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
a = [1, 8, 2187, 2351104, 6153518125, 31779658925496, 287364845865893467, 4200677982722915635200, 93566442152660422280250537];
\\ exp( n^4*A(x) )*(1-n^4*x)
print(Vec( exp( 1^4*sum(i=1, #a, a[i]*x^i) )*(1 - 1^4*x + O(x^10))));
print(Vec( exp( 2^4*sum(i=1, #a, a[i]*x^i) )*(1 - 2^4*x + O(x^10))));
print(Vec( exp( 3^4*sum(i=1, #a, a[i]*x^i) )*(1 - 3^4*x + O(x^10))));
print(Vec( exp( 4^4*sum(i=1, #a, a[i]*x^i) )*(1 - 4^4*x + O(x^10))));
print(Vec( exp( 5^4*sum(i=1, #a, a[i]*x^i) )*(1 - 5^4*x + O(x^10))));
print(Vec( exp( 6^4*sum(i=1, #a, a[i]*x^i) )*(1 - 6^4*x + O(x^10))));
print(Vec( exp( 7^4*sum(i=1, #a, a[i]*x^i) )*(1 - 7^4*x + O(x^10))));
print(Vec( exp( 8^4*sum(i=1, #a, a[i]*x^i) )*(1 - 8^4*x + O(x^10))));
print(Vec( exp( 9^4*sum(i=1, #a, a[i]*x^i) )*(1 - 9^4*x + O(x^10))));



