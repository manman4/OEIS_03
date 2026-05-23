\\ ==========================================
\\ 1. 定義通りの順次検索による d(n) (検算用)
\\ ==========================================
d_naive(n) = {
  if(n == 0, return(0));
  my(S = Set([0]));
  my(last = 0);
  my(k = 1);
  while(1,
    my(nxt = last - k);
    if(nxt >= 0 && !setsearch(S, nxt),
      S = setunion(S, [nxt]);
      last = nxt;
    ,
      nxt = last + k - 1;
      S = setunion(S, [nxt]);
      last = nxt;
    );
    if(last == n, return(k));
    k++;
  );
};
for(n=0, 100, print1(d_naive(n), ", "));

\\ ==========================================
\\ 10階線形漸化式 C_k, NK_k を用いた d(n) の高速直接計算 (バグ完全修正版)
\\ ==========================================
d_fast_with_gf(n) = {
  \\ ① 真の例外は極初期の2点のみに縮小
  if(n == 0, return(0));
  if(n == 1, return(2));
  
  \\ ② N_1=3, N_2=4 を包含した拡張初期値（例外を吸収）
  my(C  = List([3, 6, 10, 19, 31, 58, 96, 181, 303, 574, 966, 1835]));
  my(NK = List([3, 4, 5, 10, 13, 28, 39, 86, 123, 272, 393, 870]));
  
  \\ --------------------------------------------------
  \\ [フェーズ1] 動的拡張
  \\ --------------------------------------------------
  my(k = 12);
  while(C[#C-1] - NK[#C] <= n || C[#C-2] - NK[#C-1] <= n,
    k++;
    my(next_C  = 5*C[k-2]  - 6*C[k-4]  + 3*C[k-8]  - C[k-10]);
    my(next_NK = 5*NK[k-2] - 6*NK[k-4] + 3*NK[k-8] - NK[k-10]);
    
    listput(C, next_C);
    listput(NK, next_NK);
  );
  
  \\ --------------------------------------------------
  \\ [フェーズ2] 高速検索
  \\ --------------------------------------------------
  for(check_idx = 1, #C - 1,
    if(n == C[check_idx],
      return(NK[check_idx]);
    );
    
    my(k_cand = C[check_idx] - n);
    
    if(k_cand > NK[check_idx] && k_cand < NK[check_idx+1],
      if((k_cand % 2) != (check_idx % 2),
        return(k_cand);
      );
    );
  );
  
  return(0);
};

\\ ==========================================
\\ 3. テスト実行
\\ ==========================================
{
  print("n \t d_naive(n) \t d_fast(n) \t Match?");
  print("---------------------------------------------------------");
  
  my(all_passed = 1);
  \\ 前回のバグの原因となった 866 付近を完全に包含する範囲 (n=0 から 1000) でテスト
  for(n=0, 1000,
    my(val_naive = d_naive(n));
    my(val_fast = d_fast_with_gf(n));
    
    my(status = "OK");
    if(val_naive != val_fast,
      status = "ERROR";
      all_passed = 0;
    );
    
    print(n, " \t ", val_naive, " \t\t ", val_fast, " \t\t ", status);

  );
  
  print("---------------------------------------------------------");
  if(all_passed,
    print("すべてのテストが成功しました！ d(n) の母関数モデルも完璧です。");
  ,
    print("エラーが発生しました。修正が必要です。");
  );
}
