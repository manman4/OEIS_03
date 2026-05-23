\\ ==========================================
\\ a(0) から a(N) までの全項をベクトルで返す関数
\\ ==========================================
a_naive_vector(N) = {
  my(S = Set([0]));
  my(last = 0);
  my(res = vector(N + 1)); \\ a(0) から a(N) までの N+1 個の領域を確保
  res[1] = 0;              \\ res[1] が a(0) に相当 (PARIは1-indexedのため)
  
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
    res[i + 1] = last;     \\ 各ステップ a(i) の値を格納
  );
  return(res);
};

M=8970 - 1;
v=a_naive_vector(M);
for(n=0, M, write("b396346.txt", n, " ", v[n+1]));

