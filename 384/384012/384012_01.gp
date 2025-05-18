b(n) = {
  my(sum = 0);
  for(i=0, n,
    for(j=0, n,
      for(k=0, n,
        if(i + j + k == 2*n,
          sum += abs(stirling(n+1, i+1, 1) * stirling(n+1, j+1, 1) * stirling(n+1, k+1, 1));
        )
      )
    )
  );
  sum;
};

for(n=0, 20, print1(b(n),", "));

a(n) = sum(i=0, n, sum(j=0, 2*n-i, abs(stirling(n+1, i+1, 1)*stirling(n+1, j+1, 1)*stirling(n+1, 2*n-i-j+1, 1))));
for(n=0, 20, print1(a(n)-b(n),", "));
