# Fuji_log_parser

功能簡介: (1)collector:於程式運作期間即時備存每次對位使用的Mark_ID.tif檔，並依生產日期/產品名/層別/批號/流水號歸檔。(2)classifier:將Mark_ID.tif檔依黑白靶/外型/照明類型/尺寸做粗分類，並計算相同類別內靶點間相似度。(3)score_matrix:將相同類別內Mark_ID.tif檔進一步依彼此間相似度做細分類，並將同類別歸檔。以下為詳細說明。

## 1.Fuji曝光機 product log產生流程
### 1-1.Product log 製作流程圖
![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/exposurer_flowchart.jpg?raw=true) 

Fuji曝光機將所有對位參數儲存於Mark_ID.tif檔內，人員可於生產期間依需求任意修改，但設備未紀錄其修改履歷。由上方流程圖可知每次設備生產時均會產生一份AF.log檔，其檔名包含生產時間。故可藉由不斷檢查有無新的AF.log檔產生，來確認是否有對位事件發生，並啟動程式備存產品資料與Mark_ID.tif。

## 2.Collector
### 2-1.Collector 執行流程圖
![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/collector_flowchart.png?raw=true) 


如上節所述，Collector於執行期間會不斷搜尋設備端logshare資料夾內，是否有較上次紀錄時間更晚產生的AF.log檔。當新的AF.log檔產生時，再依序讀取job info.xml、jobs.csv、Mark_ID.tif取得生產時產品資料與對位參數。

### 2-2.Collector 輸出形式
![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/collector.png?raw=true)

每次程式取得產品資料與對位參數會將其輸出至指定路徑(上游側:up_output_path/下游側:dn_output_path)，包含以下內容:

1. 指定路徑下:以產品名+層別名命名的資料夾。
2. 指定路徑/產品名+層別名資料夾下:以生產日期+批號+產品名+層別名+流水號命名的Mark_ID.tif備存檔。
3. 指定路徑/產品名+層別名資料夾下:包含所有生產資訊與對位參數的template_log.csv檔。

### 2-3.Collector 使用說明
1. 編譯時須將collector.cpp、tinyxml2.cpp、tinyxml2.h加入同一個專案內。

![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/collector_members.png?raw=true)

2. 依設備IP Address修正collector.cpp內up_logshare_path、dn_logshare_path、up_jobs_path、dn_jobs_path、up_template_path、dn_template_path所指定路徑。
3. 依需求修正collector.cpp內up_output_path、dn_output_path指定的輸出路徑。

![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/collector_dir_path.png?raw=true)

## 3.Classifier
### 3-1.Classifier 執行流程圖
![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/classifier_flowchart.png?raw=true) 

接續collector蒐集的結果，
