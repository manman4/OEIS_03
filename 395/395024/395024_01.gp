{
  my(N = 11);
  my(v = vector(N), x = 'x);

  for(n = 1, N,
    my(An_prev = sum(i=1, n-1, v[i]*x^i));
    my(Q_prev = -n * An_prev + O(x^(n+1)));
    v[n] = polcoeff(exp(Q_prev)/(1-n^2*x)^n, n) / n;
  );

  \\ 表の各行を出力
  for(n = 1, N,
    my(An = sum(i=1, N, v[i]*x^i));
    my(En = exp(-n * An)/(1-n^2*x + O(x^(N+1)))^n);
    
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
a = {[1,17,1575,363246,154382680,104049305139,
 101707429423586,135888860833475576,
 237844016039201137308,528153729820573548263540,
 1451019795226044583939161675,
 4833040079652570412931114352522,
 19192370042391531000441950190742764,
 89595675606598689931615051679687288042]};
\\ exp( -n*A(x) )/(1-n^2*x)^n を出力
print(Vec( exp( -1*sum(i=1, #a, a[i]*x^i) )/(1 - 1^2*x + O(x^10))^1));
print(Vec( exp( -2*sum(i=1, #a, a[i]*x^i) )/(1 - 2^2*x + O(x^10))^2));
print(Vec( exp( -3*sum(i=1, #a, a[i]*x^i) )/(1 - 3^2*x + O(x^10))^3));
print(Vec( exp( -4*sum(i=1, #a, a[i]*x^i) )/(1 - 4^2*x + O(x^10))^4));
print(Vec( exp( -5*sum(i=1, #a, a[i]*x^i) )/(1 - 5^2*x + O(x^10))^5));
print(Vec( exp( -6*sum(i=1, #a, a[i]*x^i) )/(1 - 6^2*x + O(x^10))^6));
print(Vec( exp( -7*sum(i=1, #a, a[i]*x^i) )/(1 - 7^2*x + O(x^10))^7));
print(Vec(exp(-8*log(1 - 8^2*x) - 8*sum(i=1, #a, a[i]*x^i)) + O(x^10)));
print(Vec(exp(-9*log(1 - 9^2*x) - 9*sum(i=1, #a, a[i]*x^i)) + O(x^10)));

