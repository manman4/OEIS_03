#!/usr/bin/env ruby
# frozen_string_literal: true
#
# 349257_03.rb -- A349257 を p 進(素数冪)制約つき分枝限定で求める。
#
# 問題:
#   [n] = {1,...,n} の順列 p に対し S = Σ_{k=1..n} k/p(k)。
#   S として表せる最大の整数 a(n) を求める。
#
# 定式化:
#   q = p^{-1}, L = lcm(1,...,n), w_j = L/j と置くと j=p(k) の置換で
#       Σ_k k/p(k) = Σ_j q(j)/j,   スケール和 S = Σ_j q(j)*w_j
#   となり、元の和が整数 ⟺ L | S。以後 q(j) を j=1..n に割り当てて S を
#   直接最大化する（349257_02.rb の meet-in-the-middle とは別方式）。
#
# 整数性の分解（同値変形。必要条件ではなく必要十分）:
#   a_r = v_r(L) = ⌊log_r n⌋, P_r = r^{a_r} と置くと L = Π_{r≤n 素数} P_r で
#   各 P_r は互いに素。中国剰余定理より
#       L | S  ⟺  ∀r: S ≡ 0 (mod P_r).
#   r ∤ j なら v_r(w_j) = a_r - v_r(j) = a_r すなわち P_r | w_j なので、その項は
#   mod P_r で消える。よって各素数 r ごとに
#       Σ_{j: r|j} q(j)*w_j ≡ 0 (mod P_r)          … (1)
#   これを「素数制約」と呼ぶ。(1) を全素数で満たすことと L | S は同値。
#   注意: (1) は q が順列であることを使っていない純粋な合同条件なので、
#   単射性（各値を1回だけ使う）は別途 mask で管理する。
#
# 探索:
#   * 各制約 r について残余 Σ(既割当分) mod P_r を保持する。ある制約に属する
#     未割当の分母が1個だけになったら、その分母の値は残余を 0 にするものに
#     限られる（candidate_allowed）。この「最後の1個」を優先的に選ぶ。
#   * 並べ替え不等式による上界（残りの大きい値を大きい重みに対応）で、
#     その枝が現在の暫定最良値を超えられないなら打ち切る。
#   * 恒等順列 q(j)=j が S = n*L を与えるので暫定最良値の初期値は n。
#
# 使い方:
#   ruby 349257_03.rb            # a(0)..a(25) を出力
#   ruby 349257_03.rb --upto N   # a(0)..a(N)
#   ruby 349257_03.rb --term N   # a(N) のみ
#   ruby 349257_03.rb --witness --term N   # 達成する順列も表示
#   ruby 349257_03.rb --check    # 既知値および全順列直接探索と照合

DEFAULT_MAX_N = 25
KNOWN = [0, 1, 2, 3, 6, 7, 10, 11, 15, 18, 21, 22,
         27, 28, 32, 36, 40, 41, 46, 47].freeze
DIRECT_CHECK_MAX_N = 8

def prime?(v)
  return false if v < 2
  return v == 2 if v.even?
  d = 3
  while d * d <= v
    return false if (v % d).zero?
    d += 2
  end
  true
end

# 素数制約 (1) を素数ごとに構築する。
def build_constraints(n, weight)
  (2..n).select { |r| prime?(r) }.map do |r|
    modulus = r
    modulus *= r while modulus <= n / r          # P_r = n 以下の最大の r 冪
    js = (r..n).step(r).to_a                     # r | j なる分母
    mask = js.reduce(0) { |m, j| m | (1 << (j - 1)) }
    coefficient = Array.new(n + 1, 0)
    js.each { |j| coefficient[j] = weight[j] % modulus }
    { modulus: modulus, mask: mask, coefficient: coefficient }
  end
end

class Solver
  attr_reader :nodes

  def initialize(n)
    @n = n
    @lcm = (1..n).reduce(1) { |a, d| a.lcm(d) }
    @weight = Array.new(n + 1, 0)
    (1..n).each { |j| @weight[j] = @lcm / j }
    @constraints = build_constraints(n, @weight)
    @residue = Array.new(@constraints.length, 0)
    @inverse = Array.new(n + 1, 0)
    @best = n                                    # 恒等順列による下界
    @best_inverse = (0..n).to_a                  # q(j)=j
    @nodes = 0
  end

  def solve
    full = (1 << @n) - 1
    maximize(full, full, 0)
    [@best, @best_inverse]
  end

  private

  # 残りの割り当てが到達しうるスケール和の最大（並べ替え不等式・厳密）。
  def maximum_completion(denominators, numerators)
    values = []
    (1..@n).each { |v| values << v if numerators[v - 1] == 1 }
    total = 0
    rank = 0
    (1..@n).each do |j|
      next if denominators[j - 1].zero?
      total += values[values.length - 1 - rank] * @weight[j]
      rank += 1
    end
    total
  end

  def can_improve?(denominators, numerators, scaled_sum)
    upper = scaled_sum + maximum_completion(denominators, numerators)
    upper / @lcm > @best
  end

  # 制約 c のうち未割当分母が「この分母だけ」なら、残余を 0 にする値のみ許す。
  def candidate_allowed?(denominators, denominator, value)
    @constraints.each_with_index do |c, i|
      next if (c[:mask] & (1 << (denominator - 1))).zero?
      next unless (denominators & c[:mask]).digits(2).sum == 1
      residue = (@residue[i] + value * c[:coefficient][denominator]) % c[:modulus]
      return false unless residue.zero?
    end
    true
  end

  # 次に値を決める分母を選ぶ。制約の「最後の1個」を優先し、なければ
  # 未完了の最小グループの制約内から、係数と法の gcd が大きいものを先に。
  def choose_denominator(denominators, numerators)
    forced = 0
    active = nil
    smallest = @n + 1

    @constraints.each_with_index do |c, i|
      remaining = denominators & c[:mask]
      count = remaining.digits(2).sum
      if count.zero?
        return [nil, true] unless @residue[i].zero?
      elsif count == 1
        forced |= remaining
      elsif count < smallest ||
            (count == smallest && c[:modulus] > @constraints[active][:modulus])
        smallest = count
        active = i
      end
    end

    unless forced.zero?
      best_denominator = nil
      best_count = @n + 1
      (1..@n).each do |d|
        next if forced[d - 1].zero?
        count = (1..@n).count do |v|
          numerators[v - 1] == 1 && candidate_allowed?(denominators, d, v)
        end
        if count < best_count
          best_count = count
          best_denominator = d
        end
      end
      return [nil, true] if best_count.zero?
      return [best_denominator, false]
    end

    if active
      c = @constraints[active]
      best_denominator = nil
      best_gcd = -1
      (1..@n).each do |d|
        next if (denominators & c[:mask])[d - 1].zero?
        g = c[:coefficient][d].gcd(c[:modulus])
        if g > best_gcd
          best_gcd = g
          best_denominator = d
        end
      end
      return [best_denominator, false]
    end

    [(1..@n).find { |d| denominators[d - 1] == 1 }, false]
  end

  def record(scaled_sum)
    raise "p 進制約が非整数和を通した" unless (scaled_sum % @lcm).zero?

    value = scaled_sum / @lcm
    return if value <= @best

    @best = value
    @best_inverse = @inverse.dup
  end

  def maximize(denominators, numerators, scaled_sum)
    @nodes += 1
    return record(scaled_sum) if denominators.zero?
    return unless can_improve?(denominators, numerators, scaled_sum)

    denominator, impossible = choose_denominator(denominators, numerators)
    return if impossible

    bit_d = 1 << (denominator - 1)
    @n.downto(1) do |value|                      # 大きい値から＝和が大きい順
      bit_v = 1 << (value - 1)
      next if (numerators & bit_v).zero?
      next unless candidate_allowed?(denominators, denominator, value)

      saved = @residue.dup
      @constraints.each_with_index do |c, i|
        next if (c[:mask] & bit_d).zero?
        @residue[i] = (@residue[i] + value * c[:coefficient][denominator]) %
                      c[:modulus]
      end
      @inverse[denominator] = value

      maximize(denominators ^ bit_d, numerators ^ bit_v,
               scaled_sum + value * @weight[denominator])

      @residue = saved
      @inverse[denominator] = 0
    end
  end
end

def term(n)
  return [0, []] if n.zero?

  best, inverse = Solver.new(n).solve
  [best, inverse]
end

# 全順列の直接探索（小さい n の照合用。独立実装）。
def direct_term(n)
  return 0 if n.zero?

  l = (1..n).reduce(1) { |a, d| a.lcm(d) }
  best = 0
  (1..n).to_a.permutation do |q|
    sum = (1..n).sum { |j| q[j - 1] * (l / j) }
    best = sum / l if (sum % l).zero? && sum / l > best
  end
  best
end

def verify_witness(inverse, n, value)
  q = inverse[1..n]
  raise "witness が順列でない" unless q.sort == (1..n).to_a

  s = (1..n).sum { |j| Rational(q[j - 1], j) }   # Rational で厳密に再検算
  raise "witness の和が一致しない: #{s} != #{value}" unless s == value
end

mode = :upto
limit = DEFAULT_MAX_N
witness = false
check = false

args = ARGV.dup
until args.empty?
  case (arg = args.shift)
  when "--upto" then mode = :upto; limit = Integer(args.shift)
  when "--term" then mode = :term; limit = Integer(args.shift)
  when "--witness" then witness = true
  when "--check" then check = true; mode = :upto; limit = KNOWN.length - 1
  when "-h", "--help"
    puts "Usage: 349257_03.rb [--upto N | --term N | --check] [--witness]"
    exit 0
  else
    warn "error: unknown option: #{arg}"
    exit 1
  end
end
raise ArgumentError, "n must be >= 0" if limit.negative?

first = mode == :term ? limit : 0
results = []
(first..limit).each do |n|
  value, inverse = term(n)
  if n <= KNOWN.length - 1 && value != KNOWN[n]
    warn "error: A349257 mismatch at n=#{n}: got #{value}, expected #{KNOWN[n]}"
    exit 1
  end
  verify_witness(inverse, n, value) unless n.zero?
  if check && n <= DIRECT_CHECK_MAX_N && direct_term(n) != value
    warn "error: direct mismatch at n=#{n}"
    exit 1
  end

  results << value
  if witness && !n.zero?
    q = inverse[1..n]
    p_perm = Array.new(n + 1, 0)
    (1..n).each { |j| p_perm[q[j - 1]] = j }
    warn "n=#{n} q=p^-1: #{q.inspect}"
    warn "n=#{n} p:      #{p_perm[1..n].inspect}"
  end
end

puts mode == :term ? results.first.to_s : results.join(", ")
warn "check passed: A349257(0..#{KNOWN.length - 1}), " \
     "direct permutations through n=#{DIRECT_CHECK_MAX_N}" if check
