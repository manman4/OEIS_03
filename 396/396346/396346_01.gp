\\ ==========================================
\\ 定義通りの計算 
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
