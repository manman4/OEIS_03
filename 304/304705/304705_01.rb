@memo = {}

def b(n, r, i, t)
  # n: 残りの和
  # r: 直前の比率 (d_{t-1} / (t-1))
  # i: 今回検討するパーツの値
  # t: 現在のインデックス (1, 2, ...)
  state = [n, r, i, t]
  return @memo[state] if @memo.key?(state)

  if n == 0
    return 1
  end
  if i > n
    return 0
  end

  # パーツ i を使わない場合 (iを1増やす)
  res = b(n, r, i + 1, t)

  # パーツ i を使う場合
  # 条件: i/t <= r (比率減少) かつ i >= 前回のパーツ (iのループで担保)
  if i.to_f / t <= r
    res += b(n - i, i.to_f / t, i, t + 1)
  end

  @memo[state] = res
end

def a(n)
  @memo = {}
  # 初回: d1 は n以下、比率rはn(十分大きい値)、インデックスt=1
  b(n, n.to_f, 1, 1)
end

n = 200
digit_limit = 1000

(0..n).each{|i|
  j = a(i)
  break if j.to_s.size > digit_limit
  print i
  print ' '
  puts j
}
