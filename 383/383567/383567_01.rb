# T(n,k) is the number of lattice paths from (0,0) to (k,n-k) using steps (2,0),(0,2),(5,5).
def A(n, k)
  # 配列を初期化 (k+1)*(n+1) サイズ
  dp = Array.new(k + 1){Array.new(n + 1, 0)}
  dp[0][0] = 1

  (0..k).each{|y|
    (0..n).each{|x|
      next if dp[y][x] == 0
      # (2,0) のステップ
      dp[y][x + 2] += dp[y][x] if x + 2 <= n
      # (0,2) のステップ
      dp[y + 2][x] += dp[y][x] if y + 2 <= k
      # (5,5) のステップ
      dp[y + 5][x + 5] += dp[y][x] if y + 5 <= k && x + 5 <= n
    }
  }
  dp[k][n]
end

ary = []
(0..12).each{|i|
  (0..i).each{|j|
    ary << A(j, i-j)
  }
}
p ary

# Main diagonal gives A383568.
p (0..25).map{|i| A(i, i)}