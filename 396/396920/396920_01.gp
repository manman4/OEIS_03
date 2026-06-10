{
  my(N = 6);
  print("--- 1. B_3(x) = x * exp( B_3(B_3(B_3(x))) ) の計算 ---");
  my(B = x + O(x^2));
  for(i = 1, N,
    my(B2 = subst(B, x, B));   \\ B(B(x))
    my(B3 = subst(B, x, B2));  \\ B(B(B(x)))
    B = x * exp(B3) + O(x^(N+1));
  );
  print("B_3(x) = ", B);
  print();


  print("--- 2. A_3(x) = exp(B_3(x)) から数列の抽出 ---");
  my(A = exp(B));
  print("A_3(x) = ", A);
  print("数列 a_n (n=0..6): ", Vec(serlaplace(A)));
  print();


  print("--- 3. 関数方程式 A( x / A(log(A(x))) ) = exp(x) の検証 ---");
  my(lnA   = log(A));              \\ 1回の ln A(x)
  my(inner = subst(A, x, lnA));    \\ A( ln A(x) )
  my(arg   = x / inner);           \\ 分母に配置: x / A( ln A(x) )
  my(LHS   = subst(A, x, arg));    \\ 左辺全体
  my(RHS   = exp(x) + O(x^(N+1))); \\ 右辺: exp(x)

  print("LHS (左辺) = ", LHS);
  print("RHS (右辺) = ", RHS);
  print();
  print("LHS と RHS は完全に一致するか？ -> ", LHS == RHS);
}

print("--------------------------------------------------");

a(n, l) ={
  my(L = l);
  my(N = n);
  print("=== 計算開始: L = ", L, ", N = ", N, " ===");

  \\ 1. B(x) の不動点反復
  my(B_func = x + O(x^2));
  for(step = 1, N,
    my(B_iter = x + O(x^(N+1)));
    for(idx = 1, L, B_iter = subst(B_func, x, B_iter));
    B_func = x * exp(B_iter) + O(x^(N+1));
  );
  print("B_l(x) = ", B_func);
  print(Vec(serlaplace(B_func)));
  print();
  my(A_func = exp(B_func));
  
  \\ ★ Vec を Vecrev に修正（低次から順に並べる）
  print(Vec(serlaplace(A_func)));

  \\ 2. 分母の入れ子構造の自動構築
  my(v_var = x + O(x^(N+1)));
  if(L > 2,
    for(k_idx = 1, L - 2,
      v_var = log(subst(A_func, x, v_var)); 
    );
  );
  my(denom_func = subst(A_func, x, v_var)); 
  
  \\ 3. 関数方程式の検証
  my(arg_poly = x / denom_func);
  my(LHS_poly = subst(A_func, x, arg_poly));
  my(RHS_poly = exp(x) + O(x^(N+1)));
  
  print("LHS == RHS ? -> ", LHS_poly == RHS_poly);
  print("--------------------------------------------------");
};
\\ a(20, 0);
\\ a(20, 1);
a(20, 2);
a(20, 3);
a(20, 4);
a(20, 5);
a(20, 6);
