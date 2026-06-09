# encoding: utf-8
#
# a(n): 群数列による O(1) 計算
#
# 構造:
#   a(0) = 4  (特殊)
#
#   輪 k (k >= 1): 8k 項
#     開始位置: n = 4k(k-1) + 1
#     終了位置: n = 4k(k+1)
#     base(k)  = 2k - 1
#
#   輪k内位置 p = (n - 4k(k-1) - 1) mod 4k  ← 前半・後半が同一なので mod
#     p < 2k  かつ p 偶数  → a = base + 0
#     p < 2k  かつ p 奇数  → a = base + 1
#     p >= 2k かつ p 偶数  → a = base + 2
#     p >= 2k かつ p 奇数  → a = base + 1
#
# 性質:
#   a(k^2) = k  (k >= 1)
#   輪kの min = 2k-1, max = 2k+1

def a(n)
  return 4 if n == 0

  # n が属する輪 k を特定: 4k(k-1)+1 <= n <= 4k(k+1)
  k = ((-1 + Math.sqrt(1 + n)) / 2).ceil
  k += 1 while 4 * k * (k + 1) < n
  k -= 1 while k > 1 && 4 * (k-1) * k >= n

  base  = 2 * k - 1
  start = 4 * k * (k - 1) + 1
  p     = (n - start) % (4 * k)   # 前後半同一なので mod 4k

  if p < 2 * k
    p.even? ? base : base + 1
  else
    p.even? ? base + 2 : base + 1
  end
end

if __FILE__ == $0
  n = (ARGV[0] || 10).to_i

  # puts "a(k^2) = k の確認:"
  # (1..10).each { |k| print "a(#{k**2})=#{a(k**2)}  " }
  # puts

  # puts "\na(0)~a(#{n**2}):"
  (0..n*n).each_with_index do |i, _|
    print i
    print ' '
    puts a(i)
    # puts if (i + 1) % 8 == 0
  end
  # puts
end
