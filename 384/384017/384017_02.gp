\\ a(n) = Sum_{0<=i, j, k, l, m<=n and i+j+k+l+m=4*n} |Stirling1(n+1,i+1) * Stirling1(n+1,j+1) * Stirling1(n+1,k+1) * Stirling1(n+1,l+1) * Stirling1(n+1,m+1)|.

a(n) = {
  my(sum = 0);
  for(i=0, n,
    for(j=0, n,
      for(k=0, n,
        for(l=0, n,
          for(m=0, n,
            if(i + j + k + l + m == 4*n,
              sum += abs(stirling(n+1, i+1, 1) * stirling(n+1, j+1, 1) * stirling(n+1, k+1, 1) * stirling(n+1, l+1, 1) * stirling(n+1, m+1, 1));
            )
          )
        )
      )
    )
  );
  sum;
};

for(n=0, 20, print1(a(n),", "));


