  b(n) = {
    my(sum = 0);
      for(i=0, n,
        for(j=0, n,
            for(k=0, n,
              if(i + j + k == n,
                sum += abs(stirling(i+n, n, 2) * stirling(j+n, n, 2) * stirling(k+n, n, 2));
              )
            )
        )
      );
    sum;
  };

for(n=0, 20, print1(b(n),", "));

a(n) = sum(i=0, n, sum(j=0, n-i, stirling(i+n, n, 2)*stirling(j+n, n, 2)*stirling(2*n-i-j, n, 2)));
for(n=0, 20, print1(a(n)-b(n),", "));
