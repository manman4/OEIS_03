\\ a(n) = Sum_{i, j, k, l>=0 and i+j+k+l=n} Stirling2(i+n,n) * Stirling2(j+n,n) * Stirling2(k+n,n) * Stirling2(l+n,n).

a(n) = {
  my(sum = 0);
    for(i=0, n,
      for(j=0, n,
          for(k=0, n,
            for(l=0, n,
              if(i + j + k + l == n,
                sum += abs(stirling(i+n, n, 2) * stirling(j+n, n, 2) * stirling(k+n, n, 2) * stirling(l+n, n, 2));
              )
            )
          )
      )
    );
  sum;
};

for(n=0, 20, print1(a(n),", "));


