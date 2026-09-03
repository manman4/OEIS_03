a(n) = {
  my(N = 1<<n, dp = vector(1<<n), s, j, m);
  dp[1] = 1;
  for(mask = 0, N-2,
    if(dp[mask+1] == 0, next);
    s = 0; j = 1;
    for(x = 1, n, if(bittest(mask, x-1), s += x; j++));
    for(x = 1, n,
      if(bittest(mask, x-1), next);
      if(j >= 2 && j <= n-1 && s % x == 0, next);
      m = bitor(mask, 1<<(x-1));
      dp[m+1] += dp[mask+1]
    )
  );
  dp[N]
};

vector(17, n, a(n-1))