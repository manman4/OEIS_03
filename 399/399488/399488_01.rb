# Number of permutations sigma of [n] such that lcm(k, sigma(k)) >= n for 1 <= k <= n.
def a1(n)
  return 1 if n == 0
  (1..n).to_a.permutation.count { |p| p.each_with_index.all? { |v, i| (i + 1).lcm(v) >= n } }
end

# Number of permutations sigma of [n] such that lcm(k, sigma(k)) <= n for 1 <= k <= n.
def a2(n)
  return 1 if n == 0
  (1..n).to_a.permutation.count { |p| p.each_with_index.all? { |v, i| (i + 1).lcm(v) <= n } }
end


# Number of permutations sigma of [n] such that gcd(k, sigma(k)) <= n for 1 <= k <= n.
def a3(n)
  return 1 if n == 0
  (1..n).to_a.permutation.count { |p| p.each_with_index.all? { |v, i| (i + 1).gcd(v) <= n } }
end



p (0..10).map{|n| a1(n)}
p (0..10).map{|n| a2(n)}
p (0..10).map{|n| a3(n)}



# frozen_string_literal: true

module LcmPerm
  STATE_LIMIT = 5_000_000  # メモリ暴走よけ

  module_function

  # allowed[k-1] = 行 k に置ける列のビットマスク（bit(j-1) が列 j）
  def allowed_masks(n)
    full = (1 << n) - 1
    div = Array.new(n, 0)                 # div[L] = L の約数集合 (L < n)
    (1...n).each do |d|
      bit = 1 << (d - 1)
      m = d
      while m < n
        div[m] |= bit
        m += d
      end
    end
    Array.new(n) do |i|
      k = i + 1
      forb = 0
      l = k
      while l < n                          # l = k, 2k, ... < n
        forb |= div[l]
        l += k
      end
      full & ~forb
    end
  end

  def count(n, state_limit: STATE_LIMIT)
    raise ArgumentError, "n は 1 以上の整数を指定してください" unless n.is_a?(Integer) && n >= 1

    full  = (1 << n) - 1
    masks = allowed_masks(n)
    order = (0...n).sort_by { |i| [masks[i].to_s(2).count("1"), i] }  # 選択肢の少ない行から
    am    = order.map { |i| masks[i] }

    suffix = Array.new(n + 1, 0)           # 以降の行が使える列の和集合
    (n - 1).downto(0) { |i| suffix[i] = suffix[i + 1] | am[i] }

    states = { 0 => 1 }
    n.times do |i|
      a         = am[i]
      cover     = suffix[i + 1]
      lookahead = [i + 1 + 8, n].min
      nxt       = Hash.new(0)

      states.each do |mask, cnt|
        avail = a & ~mask
        while avail != 0
          bit    = avail & -avail
          avail ^= bit
          nm     = mask | bit
          rest   = full & ~nm

          next unless (rest & ~cover) == 0          # 枝刈り①: 到達不能な列が残った
          ok = true
          j  = i + 1
          while j < lookahead                        # 枝刈り②: 詰んだ行がある
            if (am[j] & rest) == 0
              ok = false
              break
            end
            j += 1
          end
          nxt[nm] += cnt if ok
        end
      end

      return 0 if nxt.empty?
      raise "状態数が上限を超えました (#{nxt.size}) : n=#{n}" if nxt.size > state_limit
      states = nxt
    end

    states[full] || 0
  end

  # 検算用（n <= 9 程度まで）
  def count_bruteforce(n)
    (1..n).to_a.permutation.count { |p|
      p.each_with_index.all? { |v, i| (i + 1).lcm(v) >= n }
    }
  end
end

if __FILE__ == $PROGRAM_NAME
  (1..20).each{|n| puts "#{n} #{LcmPerm.count(n)}"}
  p (1..20).map{|n| LcmPerm.count(n)}
end

