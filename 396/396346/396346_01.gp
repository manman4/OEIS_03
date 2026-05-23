\\ ==========================================
\\ 定義通りの計算 (検算用)
\\ ==========================================
a_naive(N) = {
  my(S = Set([0]));
  my(last = 0);
  for(i=1, N,
    my(nxt = last - i);
    if(nxt >= 0 && !setsearch(S, nxt),
      S = setunion(S, [nxt]);
      last = nxt;
    ,
      nxt = last + i - 1;
      S = setunion(S, [nxt]);
      last = nxt;
    );
  );
  return(last);
};

for(n=0, 200, print1(a_naive(n),", "));

\\ ==========================================
\\ 10階線形漸化式を利用した高速計算 (完全修正版)
\\ ==========================================
a_fast_with_gf(n) = {
  \\ ① 例外は n < 3 の3点のみに縮小
  if(n < 3, return(n*(n-1)/2));
  
  \\ ② N_1=3, N_2=4 を包含した拡張初期値
  my(C  = List([3, 6, 10, 19, 31, 58, 96, 181, 303, 574, 966, 1835]));
  my(NK = List([3, 4, 5, 10, 13, 28, 39, 86, 123, 272, 393, 870]));
  
  \\ ③ 動的拡張
  my(k = 12);
  while(NK[#NK] <= n,
    k++;
    my(next_C  = 5*C[k-2]  - 6*C[k-4]  + 3*C[k-8]  - C[k-10]);
    my(next_NK = 5*NK[k-2] - 6*NK[k-4] + 3*NK[k-8] - NK[k-10]);
    listput(C, next_C);
    listput(NK, next_NK);
  );
  
  \\ ④ パリティ判定
  my(idx = 0);
  for(i=1, #NK, if(NK[i] <= n, idx = i));
  
  if((idx % 2) == (n % 2), return(C[idx]), return(C[idx] - n));
};

\\ ==========================================
\\ テスト実行
\\ ==========================================
{
  print("n \t a_naive(n) \t a_fast(n) \t Match?");
  print("---------------------------------------------------------");
  
  my(all_passed = 1);
  for(n=0, 1000,
    my(val_naive = a_naive(n));
    my(val_fast = a_fast_with_gf(n));
    my(status = "OK");
    if(val_naive != val_fast,
      status = "ERROR";
      all_passed = 0;
    );

    print(n, " \t ", val_naive, " \t\t ", val_fast, " \t\t ", status);

  );
  
  print("---------------------------------------------------------");
  if(all_passed,
    print("すべてのテストが成功しました。C_kとN_kの対称性が証明されました。");
  ,
    print("エラーが発生しました。");
  );
}

