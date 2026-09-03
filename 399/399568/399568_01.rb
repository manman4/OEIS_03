# 1..n の順列 s のうち、j = 2,3,...,n-1 のすべてで
# s_j が Sum_{i=1..j} s_i を割り切らないものの個数
def count_chains(n)
  return 1 if n.zero?
  full = (1 << n) - 1
  memo = Array.new(1 << n)

  rec = lambda do |mask, sum, j|   # j = これから置く位置 (1-indexed)
    return 1 if mask == full
    c = memo[mask]
    return c if c

    total = 0
    1.upto(n) do |x|
      bit = 1 << (x - 1)
      next if mask & bit != 0
      # 制約がかかるのは 2 <= j <= n-1 のときだけ
      next if j >= 2 && j <= n - 1 && sum % x == 0
      total += rec.call(mask | bit, sum + x, j + 1)
    end
    memo[mask] = total
  end

  rec.call(0, 0, 1)
end

p (0..12).map{|n| count_chains(n)} 