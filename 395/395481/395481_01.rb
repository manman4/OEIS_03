# T(n,k): derangement のうち descents が k 個の個数
# このスクリプトは k=3 列のみを出力する
# 例: ruby 219836_01.rb 8  => T(8,3)

def t(n, k)
  return 0 if k < 0 || k >= n
  return (k == 0 ? 1 : 0) if n == 0

  dp = Hash.new { |h, key| h[key] = Array.new(k + 1, 0) }

  # pos = 1
  (1..n).each do |x|
    next if x == 1
    mask = 1 << (x - 1)
    dp[[mask, x]][0] = 1
  end

  # pos = 2..n
  (2..n).each do |pos|
    ndp = Hash.new { |h, key| h[key] = Array.new(k + 1, 0) }

    dp.each do |(mask, last), arr|
      (1..n).each do |x|
        bit = 1 << (x - 1)
        next if (mask & bit) != 0
        next if x == pos # fixed point禁止

        inc = (last > x) ? 1 : 0
        key = [mask | bit, x]
        tgt = ndp[key]

        max_d = k - inc
        0.upto(max_d) do |d|
          c = arr[d]
          next if c == 0
          tgt[d + inc] += c
        end
      end
    end

    dp = ndp
  end

  ans = 0
  dp.each_value { |arr| ans += arr[k] }
  ans
end

n = 12
p (0..n).map{|i| t(i, 2)}
p (0..n).map{|i| t(i, 3)}
p (0..n).map{|i| t(i, 4)}
p (0..n).map{|i| t(i, 5)}

