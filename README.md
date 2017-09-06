# Fuji_log_parser

功能簡介: (1)collector:於程式運作期間即時備存每次對位使用的Mark_ID.tif檔，並依生產日期/產品名/層別/批號/流水號歸檔。(2)classifier:將Mark_ID.tif檔依黑白靶/外型/照明類型/尺寸做粗分類，並計算相同類別內靶點間相似度。(3)score_matrix:將相同類別內Mark_ID.tif檔進一步依彼此間相似度做細分類，並將同類別歸檔。以下為詳細說明。

## 1.Fuji曝光機 product log產生流程
### 1-1.Product log 製作流程圖
![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/Fuji%20exposurer.png?raw=true) 

Fuji曝光機將所有對位參數儲存於Mark_ID.tif檔內，人員可於生產期間依需求任意修改，但設備未紀錄其修改履歷。由上方流程圖可知每次設備生產時均會產生一份AF.log檔，其檔名包含生產時間。故可藉由不斷檢查有無新的AF.log檔產生，來確認是否有對位事件發生，並啟動程式備存產品資料與Mark_ID.tif。

## 2.Collector
### 2-1.Collector 執行流程圖
![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/collector_flowchart.png?raw=true) 


如上節所述，Collector於執行期間會不斷搜尋設備端logshare資料夾內，是否有較上次紀錄時間更晚產生的AF.log檔。當新的AF.log檔產生時，再依序讀取job info.xml、jobs.csv、Mark_ID.tif取得生產時產品資料與對位參數。

### 2-2.Collector 輸出形式
![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/collector.png?raw=true)

每次程式取得產品資料與對位參數會將其輸出至指定路徑(上游側:up_output_path/下游側:dn_output_path)，包含以下內容:

1. 指定路徑下:以產品名+層別名命名的資料夾。
2. 指定路徑/產品名+層別名/生產日期_批號_產品名_層別名_流水號.tif: 以生產日期+批號+產品名+層別名+流水號命名的Mark_ID.tif備存檔。
3. 指定路徑/產品名+層別名/template_log.csv: 包含所有生產資訊與對位參數的清單。

### 2-3.Collector 使用說明
1. 編譯時須將collector.cpp、tinyxml2.cpp、tinyxml2.h加入同一個專案內。

![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/collector_members.png?raw=true)

2. 依設備IP Address修正collector.cpp內up_logshare_path、dn_logshare_path、up_jobs_path、dn_jobs_path、up_template_path、dn_template_path所指定路徑。
3. 依需求修正collector.cpp內up_output_path、dn_output_path指定的輸出路徑。

![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/collector_dir_path.png?raw=true)

## 3.Classifier
### 3-1.Classifier 執行流程圖
![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/classifier_flowchart.png?raw=true) 

接續collector蒐集的結果。首先classifier檢查每個collector指定路徑(root_dir)/產品名+層別名資料夾下，將未分類的.tif檔逐一與已分類的.tif做比較，刪除與過往重複的影像，並將新的影像編上流水號。同時製作一份清單，紀錄每個.tif檔曾經生產過的產品名、層別名與數量。

其次，classifier依tag ID讀取.tif檔內儲存的對位參數，先依黑白靶/外型/照明類型做初步分類，將相同類別的.tif檔存入指定路徑(result_dir)/照明類型_黑白靶_外型命名的資料夾內，並重新賦予流水號。最後計算同資料夾下，.tif檔影像間的相似度。

相似度計算方式使用Template matching。首先選定兩張.tif檔，其一作為template，另一張則作為image。當.tif檔內儲存的對位參數表明兩張影像內的靶點尺寸差異過大，相似度為零。如尺寸接近，則先利用Template matching找出template在image上的可能位置，接著將image轉為梯度圖，確認該位置上是否有接近靶點外形的物體存在，修正至正確位置。最後將template與image在正確位置疊合，計算強度內積(normed inner product)做為相似度。

### 3-2.Classifier 輸出形式
#### 路徑:root_dir/產品名+層別名/
![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/classified_dir.png?raw=true)

指定路徑(root_dir)/產品名+層別名資料夾經classified整理過，將包含以下內容:

1. 流水號.tif: 以流水號編號，影像不重複的Mark_ID.tif檔
2. classified_log.csv: 包含每個Mark_ID.tif檔曾生產過的產品清單；其中template_backup欄位為Mark_ID.tif檔流水號、sequence_number欄位為生產數量。

#### 路徑:result_dir/照明類型_黑白靶_外型/
![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/classifier.png?raw=true)

指定路徑(result_dir)/照明類型_黑白靶_外型資料夾內則包含以下內容:

1. 流水號.tif: 經collector蒐集，所有照明類型、黑白靶、外型相同的Mark_ID.tif檔。以流水號編號，影像不重複。
2. 流水號(score).csv: 在計算相似度時，以該流水號.tif檔作為template，其餘檔案作為image所得到的相似度。檔案中first row表示template的流水號，first column代表image的流水號。
3. 流水號(log).csv: 以該流水號.tif檔生產過的產品清單。其中lot_number欄位內不同批號以space隔開。

#### 路徑:result_dir/照明類型_黑白靶_外型/流水號(match_image)/
![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/classifier_align_result.png?raw=true)

指定路徑(result_dir)/照明類型_黑白靶_外型/流水號(match_image)資料夾內包含以下內容:

1.流水號.tif: 以資料夾名的流水號作為template，資料夾內的影像檔流水號作為image，經影像比對找出的靶點位置。影像中綠點為靶點中心，紅點為靶點邊緣。

### 3-3.Classifier 使用說明

1. 編譯時須將Classifier.cpp、Classifier.h、Pre-classify.cpp加入同一個專案內。

![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/classifier_members.png?raw=true)

2. 需安裝OpenCV library。開發使用版本為2.4.13。

3. Pre-classify.cpp內root_dir指定路徑的子路徑，應包含collector.cpp內up_output_path、dn_output_path指定路徑。

4. 依需求修正Pre-classify.cpp內result_dir指定的輸出路徑。

![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/classifier_dir_path.png?raw=true)

5. 相似度最高為1.0，最低為0。

## 4.Score_matrix
### 4-1.Score_matrix 執行流程圖
![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/score_matrix_flowchart.png?raw=true)

接續classifier蒐集的結果。score_matrix首先將classifier製作出的#(score).csv檔依流水號的順序合併，形成如下的矩陣格式。first row表示template的流水號，first column代表image的流水號。

![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/score_matrix.png?raw=true)

假設我們先選定一個閥值為0.9。接著我們判定當兩個靶點間的相似度超過這個閥值，屬於相同類別。且要求同類別的靶點內，任選一張靶點作為template，另一張作為image，計算出來的相似度都必須高過閥值。則我們可以將上述的矩陣細分出以下類別。

![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/score_matrix(color%20map).png?raw=true)

這就是score_matrix接著要完成的事情。score_matrix依原始檔內設定的閥值，將指定路徑(dest_path)/照明類型_黑白靶_外型資料夾內的.tif作細分類，並將相同類別的靶點存入指定路徑(dest_path)/照明類型_黑白靶_外型/group_流水號命名的資料夾內。存入的.tif檔將不再重新編號，且提供同類別的靶點相似度組成的子矩陣，以供確認。

### 4-2.Score_matrix 輸出形式
![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/group.png?raw=true)

指定路徑(dest_path)/照明類型_黑白靶_外型/group_流水號資料夾內包含以下內容:

1. 流水號.tif: 彼此間相似度高於閥值的Mark_ID.tif檔。
2. group_流水號.csv: 同類別的靶點相似度組成的子矩陣。first row表示template的流水號，first column代表image的流水號。

### 4-3.Score_matrix 使用說明

1. 需安裝boost/filesystem。開發使用版本為1.61。

2. score_matrix.cpp內dest_dir指定路徑的子路徑，應與Pre-classify.cpp內result_dir指定路徑相同。

![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/score_matrix_dir_path.png?raw=true)

3. 依需求修改score_matrix.cpp內閥值大小threshold。

![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/score_matrix_threshold.png?raw=true)

## 5.補充資料
### 5-1.Mark_ID.tif內Tag ID清單

程式讀取Mark_ID.tif內儲存的對位參數，即使用以下tag_ID清單。可參考tif檔標準格式讀取標籤內容。

![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/tagID_list.png)
