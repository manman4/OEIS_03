{
  my(N = 11);
  my(v = vector(N), x = 'x);

  for(n = 1, N,
    my(An_prev = sum(i=1, n-1, v[i]*x^i));
    my(Q_prev = -n * An_prev + O(x^(n+1)));
    v[n] = polcoeff(exp(Q_prev)/(1-n*x), n) / n;
  );

  \\ 表の各行を出力
  for(n = 1, N,
    my(An = sum(i=1, N, v[i]*x^i));
    my(En = exp(-n * An)/(1-n*x + O(x^(N+1))));
    
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
a = [1, 1, 3, 18, 165, 2019, 30688, 554784, 11591649, 274313325, 7242994143, 210931834662, 6713206636084, 231754182524900];
\\ exp( -log(1-n*x) - n*A(x) )を出力
print(Vec(exp(-log(1 - 1*x) - 1*sum(i=1, #a, a[i]*x^i)) + O(x^10)));
print(Vec(exp(-log(1 - 2*x) - 2*sum(i=1, #a, a[i]*x^i)) + O(x^10)));
print(Vec(exp(-log(1 - 3*x) - 3*sum(i=1, #a, a[i]*x^i)) + O(x^10)));
print(Vec(exp(-log(1 - 4*x) - 4*sum(i=1, #a, a[i]*x^i)) + O(x^10)));
print(Vec(exp(-log(1 - 5*x) - 5*sum(i=1, #a, a[i]*x^i)) + O(x^10)));
print(Vec(exp(-log(1 - 6*x) - 6*sum(i=1, #a, a[i]*x^i)) + O(x^10)));
print(Vec(exp(-log(1 - 7*x) - 7*sum(i=1, #a, a[i]*x^i)) + O(x^10)));
print(Vec(exp(-log(1 - 8*x) - 8*sum(i=1, #a, a[i]*x^i)) + O(x^10)));
print(Vec(exp(-log(1 - 9*x) - 9*sum(i=1, #a, a[i]*x^i)) + O(x^10)));

