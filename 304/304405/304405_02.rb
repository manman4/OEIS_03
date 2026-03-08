def a304405(max_n)
  @memo = {}

  # n: 残りの合計
  # s_limit: 直前のグループの和 (これ以下である必要がある)
  # d_limit: 直前のパーツの値 (これ未満である必要がある)
  def f(n, s_limit, d_limit)
    return 1 if n == 0
    
    state = [n, s_limit, d_limit]
    return @memo[state] if @memo.key?(state)

    count = 0

    # 次に使うパーツ d を選ぶ (前のパーツ d_limit より小さくする)
    (1...d_limit).each{|d|
      # パーツ d を k 個使う。その和 s = d * k
      (1..(n / d)).each{|k|
        s = d * k
        
        # 条件：現在のグループの和 s が、直前の和 s_limit 以下であること
        if s <= s_limit
          count += f(n - s, s, d)
        end
      }
    }

    @memo[state] = count
  end

  (0..max_n).map{|i|
    @memo = {}
    # 初回呼び出しは制限なし (便宜上 i+1 を指定)
    f(i, i + 1, i + 1)
  }
end

# 実行
n = 100
results = a304405(n)
(0..n).each{|i| 
  print i
  print ' '
  puts results[i]
}
