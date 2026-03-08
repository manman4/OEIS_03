def a304406(max_n)
  @memo = {}

  # n: 残りの合計
  # s_limit: 直前の成分和（これ以下にする）
  # d_min: 次に使える最小のパーツ値（これより大きくする）
  def f(n, s_limit, d_min)
    return 1 if n == 0
    
    state = [n, s_limit, d_min]
    return @memo[state] if @memo.key?(state)

    count = 0

    # パーツ d を d_min から順に試す
    (d_min..n).each do |d|
      # パーツ d を k 個使う（和 s = d * k）
      (1..(n / d)).each do |k|
        s = d * k
        
        # 条件：今回の成分和 s が直前の和 s_limit 以下であること
        if s <= s_limit
          count += f(n - s, s, d + 1)
        end
      end
    end

    @memo[state] = count
  end

  (0..max_n).map do |i|
    @memo = {}
    f(i, i, 1) # 最初は s_limit を i (最大値) に設定
  end
end

# 実行
n = 100
results = a304406(n)

(0..n).each{|i|
  print i
  print " "
  puts results[i]
}
