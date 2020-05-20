import anurbs as an
import numpy as np
import pytest
from numpy.testing import assert_almost_equal, assert_equal

if __name__ == '__main__':
    import sys
    pytest.main(sys.argv)


@pytest.fixture
def curve():
    curve_geometry = an.NurbsCurveGeometry3D(degree=4, nb_poles=8)

    curve_geometry.knots = [3, 3, 3, 3, 6.5, 10, 13.5, 17, 17, 17, 17]

    curve_geometry.poles = [
        [  0, -25, - 5],
        [-15, -15,   0],
        [  5, - 5, - 3],
        [ 15, -15,   3],
        [ 25,   0,   6],
        [ 15,  15,   6],
        [- 5, - 5, - 3],
        [-25,  15,   4],
    ]

    return curve_geometry


def test_projection(curve):
    projection = an.PointOnCurveProjection3D(curve, 1e-8)

    projection.compute([-25.1331415843, -38.9256022819, -3.2989320128])
    assert_almost_equal(projection.parameter, 3.3983282912)

    projection.compute([35.6464813397, 27.3703996918, -41.1153099924])
    assert_almost_equal(projection.parameter, 13.3339477287)

    projection.compute([-40.3995502695, 45.1689836547, -1.7412051334])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([36.117074539, 44.5183648237, 47.2049699152])
    assert_almost_equal(projection.parameter, 13.0827431544)

    projection.compute([36.8315563476, -48.7314244261, 46.3990433125])
    assert_almost_equal(projection.parameter, 9.4334414008)

    projection.compute([-39.7935307537, 1.0082708909, -48.4975476742])
    assert_almost_equal(projection.parameter, 16.7432603141)

    projection.compute([39.2152096095, -39.0656723124, -28.995046196])
    assert_almost_equal(projection.parameter, 3)

    projection.compute([-11.5997738492, 6.2506795657, 41.5889377667])
    assert_almost_equal(projection.parameter, 16.7208217289)

    projection.compute([-49.8732305131, -40.7106279818, 48.4922331285])
    assert_almost_equal(projection.parameter, 3.8219161918)

    projection.compute([-0.5889005263, 15.2143434459, -2.7129801701])
    assert_almost_equal(projection.parameter, 15.4833842886)

    projection.compute([48.969280533, 1.8857173398, -5.5880641358])
    assert_almost_equal(projection.parameter, 11.4650237679)

    projection.compute([-8.4467404794, 45.5121414715, -45.3669887015])
    assert_almost_equal(projection.parameter, 16.9062391608)

    projection.compute([30.4369597139, -1.7965056709, 48.9445074922])
    assert_almost_equal(projection.parameter, 11.6561943636)

    projection.compute([-44.3057219006, 33.0192715316, 47.8292196048])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-5.7458762805, -43.1324416274, 40.1634508698])
    assert_almost_equal(projection.parameter, 3.248644251)

    projection.compute([-40.9041742286, 3.1722395463, 4.5642140576])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-31.6555538129, -49.6355080975, -48.3113358721])
    assert_almost_equal(projection.parameter, 3.0165966193)

    projection.compute([-7.825023475, 48.957493342, 43.3268881837])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-44.9014713033, 48.0409349306, -44.2031802117])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([22.8517943401, -29.0174949817, 12.8639449658])
    assert_almost_equal(projection.parameter, 8.9331640387)

    projection.compute([27.4416171375, 22.6609359834, 15.6104371723])
    assert_almost_equal(projection.parameter, 13.0698217031)

    projection.compute([-30.2095402406, -18.2692825646, 24.9043642426])
    assert_almost_equal(projection.parameter, 4.0479717921)

    projection.compute([48.586275195, 41.7056994008, -14.2714379655])
    assert_almost_equal(projection.parameter, 13.0251600483)

    projection.compute([18.275234135, -3.5222361579, -22.7704009846])
    assert_almost_equal(projection.parameter, 7.3009323651)

    projection.compute([6.3712748496, -41.5209055373, -17.2412156906])
    assert_almost_equal(projection.parameter, 3)

    projection.compute([1.2251402024, -22.859654237, -49.4462563188])
    assert_almost_equal(projection.parameter, 3)

    projection.compute([-0.2169936664, 45.897933932, 7.9948189473])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-10.3139075266, 7.8029314325, -49.8249060008])
    assert_almost_equal(projection.parameter, 16.1366226545)

    projection.compute([42.6518123563, -7.5629428763, -48.0427275868])
    assert_almost_equal(projection.parameter, 9.5900401609)

    projection.compute([-45.737014057, -28.2994790833, -30.3337322922])
    assert_almost_equal(projection.parameter, 3.6872676663)

    projection.compute([1.2162083533, -9.9415968917, 14.8779786028])
    assert_almost_equal(projection.parameter, 5.8379685342)

    projection.compute([29.9975908268, 19.9978367751, -14.8495243233])
    assert_almost_equal(projection.parameter, 13.1388148975)

    projection.compute([-16.2058498553, -12.1394114393, -24.9289664323])
    assert_almost_equal(projection.parameter, 3.9213943448)

    projection.compute([3.4080482802, -48.8883231296, -43.8845983678])
    assert_almost_equal(projection.parameter, 3)

    projection.compute([46.1560620908, -41.6277617643, 8.0593691012])
    assert_almost_equal(projection.parameter, 9.3884196883)

    projection.compute([20.6848680837, 44.7835938049, -28.666853336])
    assert_almost_equal(projection.parameter, 13.9169158965)

    projection.compute([-48.5924598754, 31.7137622655, -23.0120238722])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([42.6690408926, 21.1015188466, 39.1260346347])
    assert_almost_equal(projection.parameter, 12.5371008351)

    projection.compute([-9.4118899942, 43.2968541949, -20.261988449])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-49.8420792631, 22.5401981606, -49.1368522864])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-26.8725741547, 28.7433622306, 19.6908688032])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-47.2028514823, -47.8318335478, 15.5816407248])
    assert_almost_equal(projection.parameter, 3.6148767131)

    projection.compute([16.8426310023, 22.1283477601, 43.479231416])
    assert_almost_equal(projection.parameter, 13.2391795456)

    projection.compute([-46.7923978329, -1.5107623076, 43.8335186307])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([46.2800507882, -1.1699410394, 23.3208604033])
    assert_almost_equal(projection.parameter, 11.4617729153)

    projection.compute([-25.1075640671, 16.0016334923, -20.8414799398])
    assert_almost_equal(projection.parameter, 16.8481879785)

    projection.compute([-41.8020922652, 49.4673997161, 22.9006189261])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-23.2073077342, -44.9300117301, 22.010030305])
    assert_almost_equal(projection.parameter, 3.429348821)

    projection.compute([37.8241089582, -17.2122999407, -26.5997939168])
    assert_almost_equal(projection.parameter, 9.2337126983)

    projection.compute([2.5119125622, 24.8735006316, -33.4853518212])
    assert_almost_equal(projection.parameter, 15.4768984221)

    projection.compute([42.3360173555, -22.3200439812, 37.2103834])
    assert_almost_equal(projection.parameter, 10.3336146389)

    projection.compute([24.6305152656, 47.4646406236, 24.1349146581])
    assert_almost_equal(projection.parameter, 13.4305844796)

    projection.compute([10.5149867295, -15.3445231101, 39.6555222057])
    assert_almost_equal(projection.parameter, 9.6202379101)

    projection.compute([-0.6580345103, 17.6498819923, 21.9638905823])
    assert_almost_equal(projection.parameter, 14.6181118337)

    projection.compute([21.9565900378, 4.854384649, -46.3083175459])
    assert_almost_equal(projection.parameter, 6.9412270659)

    projection.compute([47.2674666426, 49.1388321385, 13.4482732338])
    assert_almost_equal(projection.parameter, 13.0568860076)

    projection.compute([-25.7504245153, 24.6689192833, -43.3493452116])
    assert_almost_equal(projection.parameter, 16.8806165748)

    projection.compute([-30.1640459244, 6.0843163431, 26.2018722371])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([29.3484592714, 46.1486992408, -8.1712008725])
    assert_almost_equal(projection.parameter, 13.495386228)

    projection.compute([48.3516445841, 45.3574198277, -48.7276976457])
    assert_almost_equal(projection.parameter, 13.2416276365)

    projection.compute([-45.9047522377, -19.3977520193, 2.7042823158])
    assert_almost_equal(projection.parameter, 3.947078163)

    projection.compute([-48.2935732223, 3.1715559089, -21.2307443243])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([19.803537554, 1.7730305678, 2.7095494572])
    assert_almost_equal(projection.parameter, 11.9476022633)

    projection.compute([12.4297294125, -49.8548706993, 4.3646752156])
    assert_almost_equal(projection.parameter, 3)

    projection.compute([-23.3992451212, -33.0813171263, -24.6736706582])
    assert_almost_equal(projection.parameter, 3.3112300201)

    projection.compute([-18.7366216764, 11.0967950249, 8.6394815979])
    assert_almost_equal(projection.parameter, 16.7971490178)

    projection.compute([-26.0479715076, -28.0749771642, 46.442157075])
    assert_almost_equal(projection.parameter, 3.9431939675)

    projection.compute([4.5678507325, -22.0657207407, -2.8295629904])
    assert_almost_equal(projection.parameter, 3)

    projection.compute([47.0529675004, -49.7124435844, 26.3974328415])
    assert_almost_equal(projection.parameter, 9.4338494463)

    projection.compute([-26.2698389014, -14.3729289828, -44.3610589459])
    assert_almost_equal(projection.parameter, 3.7178330567)

    projection.compute([20.170082464, 3.6481735081, 24.0622370383])
    assert_almost_equal(projection.parameter, 12.3083751719)

    projection.compute([-34.3673068957, -16.577460741, -17.3887349513])
    assert_almost_equal(projection.parameter, 3.9002353736)

    projection.compute([48.5242923249, 23.3141597702, -0.400653505])
    assert_almost_equal(projection.parameter, 12.5578436829)

    projection.compute([46.056583941, -22.4939376919, -6.313336434])
    assert_almost_equal(projection.parameter, 9.8354127278)

    projection.compute([-16.0577103338, 28.7644069077, 44.1796470406])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-49.6267911045, 23.8918445883, 27.4761605437])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-12.9306530873, 47.8111545079, 23.2428442097])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-38.7378187332, 48.3537688844, -21.8518963418])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-45.7704795039, -29.7681998367, 17.3147712682])
    assert_almost_equal(projection.parameter, 3.8500988476)

    projection.compute([-3.9792966815, -33.0479217614, 17.9478132482])
    assert_almost_equal(projection.parameter, 3.2136732553)

    projection.compute([13.1598295938, 48.6314966803, -46.5716411344])
    assert_almost_equal(projection.parameter, 14.4196487184)

    projection.compute([-20.2005061182, -4.6676250895, -2.054497065])
    assert_almost_equal(projection.parameter, 16.4492271092)

    projection.compute([34.9103078642, -42.3299725132, -10.2239740362])
    assert_almost_equal(projection.parameter, 3)

    projection.compute([-20.2698560759, 36.5470800952, -1.4485126135])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-41.7693495246, 21.7847013249, -0.4240519136])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([41.6287406309, 13.4751787239, -27.3240220627])
    assert_almost_equal(projection.parameter, 12.3286650542)

    projection.compute([-28.3444108806, -48.405116982, 49.6433370279])
    assert_almost_equal(projection.parameter, 3.5989999055)

    projection.compute([10.5399498532, -40.6306048812, 29.2711783104])
    assert_almost_equal(projection.parameter, 3)

    projection.compute([44.1834146595, -21.1933777068, 13.3290060625])
    assert_almost_equal(projection.parameter, 10.1362505804)

    projection.compute([21.063313806, -25.9638172462, -35.295762953])
    assert_almost_equal(projection.parameter, 3)

    projection.compute([-41.1744665313, -49.737137556, -16.550619419])
    assert_almost_equal(projection.parameter, 3.3804623931)

    projection.compute([9.8580917157, 16.7146294223, -20.1967504202])
    assert_almost_equal(projection.parameter, 14.7825668458)

    projection.compute([25.3265625217, -13.2317370098, -7.9272767799])
    assert_almost_equal(projection.parameter, 9.1345423503)

    projection.compute([34.0210880078, -45.3797400908, -47.5821475487])
    assert_almost_equal(projection.parameter, 3)

    projection.compute([-44.0322639393, -31.9711322347, -10.1224126109])
    assert_almost_equal(projection.parameter, 3.7075048514)

    projection.compute([-5.9085791725, -21.4804987756, 32.4249613483])
    assert_almost_equal(projection.parameter, 4.1234285025)

    projection.compute([7.0652345927, 38.8497738581, 43.4287495881])
    assert_almost_equal(projection.parameter, 13.6988253951)

    projection.compute([-29.6910216006, 41.4709048306, 32.7122103342])
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-17.4587137846, -23.6425445292, 4.9992781389])
    assert_almost_equal(projection.parameter, 3.7460748971)

    projection.compute([10.8703085039, 39.8229706054, -12.0107919266])
    assert_almost_equal(projection.parameter, 14.1342363212)


def test_projection_with_max_distance(curve):
    projection = an.PointOnCurveProjection3D(curve, 1e-8)

    projection.compute([-5.04769521, -20.83092374, -3.18189858], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3.3983282912)

    projection.compute([16.42347035, 6.21449644, 4.92331405], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 13.3339477287)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([17.39485373, 5.68580733, 5.12682974], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 13.0827431544)

    projection.compute([17.64755972, -7.6777634, 3.57406801], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 9.4334414008)

    projection.compute([-19.444162, 10.04220948, 2.29607851], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 16.7432603141)

    projection.compute([0, -25, -5], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3)

    projection.compute([-18.98724009, 9.68958312, 2.17826997], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 16.7208217289)

    projection.compute([-7.1948812, -17.2105118, -2.05508015], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3.8219161918)

    projection.compute([0.35869499, 3.38954639, 0.86797347], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 15.4833842886)

    projection.compute([20.27116196, -0.37704838, 5.2027021], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 11.4650237679)

    projection.compute([-22.89944394, 12.98320802, 3.29838071], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 16.9062391608)

    projection.compute([20.20454032, 0.45084658, 5.27716329], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 11.6561943636)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-3.54531809, -22.30850002, -3.7644091], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3.248644251)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-0.28115205, -24.81099901, -4.90603582], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3.0165966193)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([16.31144263, -8.65008224, 2.96544474], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 8.9331640387)

    projection.compute([17.44063482, 5.65392165, 5.13551393], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 13.0698217031)

    projection.compute([-7.31962461, -15.60374352, -1.69721367], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 4.0479717921)

    projection.compute([17.59577146, 5.54044702, 5.16425098], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 13.0251600483)

    projection.compute([10.47685321, -9.43849582, 0.66647651], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 7.3009323651)

    projection.compute([0, -25, -5], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3)

    projection.compute([0, -25, -5], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-8.53853107, 4.12070057, 0.52070974], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 16.1366226545)

    projection.compute([18.02146021, -7.29278745, 3.74847314], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 9.5900401609)

    projection.compute([-6.81170649, -18.27340425, -2.34054389], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3.6872676663)

    projection.compute([2.45909342, -9.46261155, -0.86639791], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 5.8379685342)

    projection.compute([17.19149609, 5.81911382, 5.08718578], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 13.1388148975)

    projection.compute([-7.3241012, -16.47639413, -1.88048339], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3.9213943448)

    projection.compute([0, -25, -5], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3)

    projection.compute([17.53600311, -7.78139428, 3.52245228], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 9.3884196883)

    projection.compute([13.5447463, 6.63472462, 4.16331378], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 13.9169158965)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([18.98708479, 4.01624784, 5.35988316], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 12.5371008351)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-6.49732412, -18.878343, -2.52004505], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3.6148767131)

    projection.compute([16.80827389, 6.03628644, 5.00805305], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 13.2391795456)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([20.2717339, -0.39113576, 5.20129127], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 11.4617729153)

    projection.compute([-21.64073868, 11.85656518, 2.91088292], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 16.8481879785)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-5.30548492, -20.53789858, -3.07465406], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3.429348821)

    projection.compute([17.13928884, -8.11323644, 3.34018294], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 9.2337126983)

    projection.compute([0.43470907, 3.39890732, 0.87843083], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 15.4768984221)

    projection.compute([19.46089932, -4.97991798, 4.46642292], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 10.3336146389)

    projection.compute([16.00776987, 6.36746022, 4.82641295], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 13.4305844796)

    projection.compute([18.09097664, -7.21421615, 3.78117839], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 9.6202379101)

    projection.compute([8.77894539, 5.53634076, 2.72995988], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 14.6181118337)

    projection.compute([8.81851914, -9.33212623, 0.18408692], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 6.9412270659)

    projection.compute([17.48606237, 5.62157263, 5.1440405], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 13.0568860076)

    projection.compute([-22.33999349, 12.4747624, 3.12304872], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 16.8806165748)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([15.71565494, 6.45280386, 4.75525986], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 13.495386228)

    projection.compute([16.7986155, 6.04122632, 5.00598848], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 13.2416276365)

    projection.compute([-7.33787274, -16.2938043, -1.83994839], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3.947078163)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([19.97327397, 1.69574836, 5.35580005], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 11.9476022633)

    projection.compute([0, -25, -5], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3)

    projection.compute([-4.2271023, -21.67780799, -3.50730652], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3.3112300201)

    projection.compute([-20.55982575, 10.93930284, 2.59842637], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 16.7971490178)

    projection.compute([-7.33628637, -16.32123595, -1.84596446], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3.9431939675)

    projection.compute([0, -25, -5], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3)

    projection.compute([17.64856239, -7.6768099, 3.57453273], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 9.4338494463)

    projection.compute([-6.92101133, -18.02505669, -2.27041278], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3.7178330567)

    projection.compute([19.45585835, 3.16126262, 5.38579239], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 12.3083751719)

    projection.compute([-7.30686518, -16.62894809, -1.91523554], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3.9002353736)

    projection.compute([18.9389529, 4.09006614, 5.35562076], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 12.5578436829)

    projection.compute([18.56098211, -6.61479883, 4.00546524], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 9.8354127278)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-7.24411306, -16.99816302, -2.00266914], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3.8500988476)

    projection.compute([-3.12928624, -22.66903898, -3.91698562], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3.2136732553)

    projection.compute([10.28913031, 5.99641497, 3.17890886], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 14.4196487184)

    projection.compute([-13.79645142, 6.30920042, 1.09720491], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 16.4492271092)

    projection.compute([0, -25, -5], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([19.41880024, 3.23987089, 5.38499669], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 12.3286650542)

    projection.compute([-6.4175836, -19.01418851, -2.56203451], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3.5989999055)

    projection.compute([0, -25, -5], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3)

    projection.compute([19.1381145, -5.66617715, 4.29341541], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 10.1362505804)

    projection.compute([0, -25, -5], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3)

    projection.compute([-4.89114966, -21.00173115, -3.24568611], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3.3804623931)

    projection.compute([7.4200226, 5.10012235, 2.34622245], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 14.7825668458)

    projection.compute([16.87434042, -8.30610008, 3.21945474], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 9.1345423503)

    projection.compute([0, -25, -5], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3)

    projection.compute([-6.88559257, -18.10850716, -2.2937462], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3.7075048514)

    projection.compute([-7.23553934, -15.11562114, -1.60626212], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 4.1234285025)

    projection.compute([14.72754983, 6.62398428, 4.49798832], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 13.6988253951)

    projection.compute([-25, 15, 4], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 17)

    projection.compute([-7.01009438, -17.79929481, -2.20846941], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 3.7460748971)

    projection.compute([12.23205283, 6.46607449, 3.77098237], max_distance=1e-2)
    assert_almost_equal(projection.parameter, 14.1342363212)
