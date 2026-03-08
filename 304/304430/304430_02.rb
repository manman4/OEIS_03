@memo = {}

def f(n, s, d)
  return 1 if n == 0
  # メモ化
  state = [n, s, d]
  return @memo[state] if @memo.key?(state)

  count = 0
  # 新しいパーツ i は直前のパーツ d より大きいものから選ぶ
  (d + 1..n).each{|i|
    # 個数 k を選ぶ。ただし和 sum = i * k は直前の和 s より小さくなければならない
    k = 1
    loop do
      sum = i * k
      break if sum >= s || sum > n
      
      count += f(n - sum, sum, i)
      k += 1
    end
  }

  @memo[state] = count
end

def a304430(n)
  @memo = {}
  f(n, n + 1, 0)
end

n =100
(0..n).each{|i|
  print i
  print ' '
  puts a304430(i)
}