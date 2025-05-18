b(n) = {
  my(sum = 0);
  for(i=0, n,
    for(j=0, n,
      for(k=0, n,
        for(l=0, n,
          if(i + j + k + l == 2*n,
            sum += abs(stirling(n, i, 1) * stirling(n, j, 1) * stirling(n, k, 1) * stirling(n, l, 1));
          )
        )
      )
    )
  );
  sum;
};

for(n=0, 20, print1(b(n),", "));

a(n) = sum(i=0, n, sum(j=0, 2*n-i, sum(k=0, 2*n-i-j, abs(stirling(n, i, 1)*stirling(n, j, 1)*stirling(n, k, 1)*stirling(n, 2*n-i-j-k, 1)))));
for(n=0, 20, print1(a(n)-b(n),", "));
