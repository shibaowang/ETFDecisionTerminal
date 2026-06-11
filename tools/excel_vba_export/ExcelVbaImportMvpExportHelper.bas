Attribute VB_Name = "ExcelVbaImportMvpExportHelper"
Option Explicit

Private Const MVP_SCHEMA_VERSION As String = "excel-vba-export/v1"
Private Const MVP_SOURCE As String = "sanitized-excel-vba-export"
Private Const SAMPLE_ACCOUNT_CODE As String = "DEMO_ACCOUNT"
Private Const SAMPLE_PORTFOLIO_CODE As String = "DEMO_PORTFOLIO"
Private Const SAMPLE_INSTRUMENT_CODE As String = "DEMO_ETF"
Private Const SAMPLE_SOURCE_LABEL As String = "SANITIZED_SAMPLE"

Public Sub ExportMvpJsonTxt()
    On Error GoTo FailClosed

    Dim suggestedFile As String
    If Len(ThisWorkbook.Path) > 0 Then
        suggestedFile = ThisWorkbook.Path & Application.PathSeparator & "TASK274_export_helper_expected_sample.json"
    Else
        suggestedFile = "TASK274_export_helper_expected_sample.json"
    End If

    Dim selectedTarget As Variant
    selectedTarget = Application.GetSaveAsFilename( _
        InitialFileName:=suggestedFile, _
        FileFilter:="JSON Files (*.json),*.json,Text Files (*.txt),*.txt")

    If VarType(selectedTarget) = vbBoolean Then
        Exit Sub
    End If

    Dim targetPath As String
    targetPath = CStr(selectedTarget)
    RequireAllowedExportExtension targetPath

    Dim payload As String
    payload = BuildMvpExportJson()
    WriteUtf8TextFile targetPath, payload

    MsgBox "MVP JSON/TXT export completed.", vbInformation, "Excel/VBA Import MVP Export"
    Exit Sub

FailClosed:
    MsgBox "MVP export stopped: " & Err.Description, vbExclamation, "Excel/VBA Import MVP Export"
End Sub

Private Function BuildMvpExportJson() As String
    Dim initialCashJson As String
    Dim tradeLogJson As String

    initialCashJson = BuildSheetJson("InitialCash", InitialCashHeaders(), False)
    tradeLogJson = BuildSheetJson("TradeLog", TradeLogHeaders(), True)

    BuildMvpExportJson = "{" & _
        JsonPair("schemaVersion", MVP_SCHEMA_VERSION) & "," & _
        JsonPair("source", MVP_SOURCE) & "," & _
        JsonPair("description", SAMPLE_SOURCE_LABEL & " Excel/VBA MVP export") & "," & _
        """sheets"":[" & initialCashJson & "," & tradeLogJson & "]" & _
        "}"
End Function

Private Function InitialCashHeaders() As Variant
    InitialCashHeaders = Array("ROW_ID", "TIME_UTC", "ACCOUNT_CODE", "PORTFOLIO_CODE", "ACTION", "AMOUNT", "CURRENCY", "MEMO")
End Function

Private Function TradeLogHeaders() As Variant
    TradeLogHeaders = Array("ROW_ID", "TRADE_TIME_UTC", "ACCOUNT_CODE", "PORTFOLIO_CODE", "INSTRUMENT_CODE", "SIDE", "QUANTITY", "PRICE", "AMOUNT", "FEE", "CASH_FLOW", "CURRENCY", "SOURCE", "MEMO")
End Function

Private Function BuildSheetJson(ByVal sheetName As String, ByVal requiredHeaders As Variant, ByVal requireDataRows As Boolean) As String
    Dim ws As Worksheet
    Set ws = RequiredSheet(sheetName)

    Dim headerColumns() As Long
    ReDim headerColumns(LBound(requiredHeaders) To UBound(requiredHeaders))

    Dim index As Long
    For index = LBound(requiredHeaders) To UBound(requiredHeaders)
        headerColumns(index) = RequiredHeaderColumn(ws, CStr(requiredHeaders(index)))
    Next index

    Dim lastRow As Long
    lastRow = LastDataRow(ws, headerColumns(LBound(headerColumns)))
    If requireDataRows And lastRow < 2 Then
        Err.Raise vbObjectError + 2743, "ExcelVbaImportMvpExportHelper", "TradeLog sheet has no data rows."
    End If

    BuildSheetJson = "{" & _
        JsonPair("name", sheetName) & "," & _
        JsonPair("sheetName", sheetName) & "," & _
        """headers"":" & JsonStringArray(requiredHeaders) & "," & _
        """rows"":" & JsonRowsArray(ws, requiredHeaders, headerColumns, lastRow) & _
        "}"
End Function

Private Function RequiredSheet(ByVal sheetName As String) As Worksheet
    On Error Resume Next
    Set RequiredSheet = ThisWorkbook.Worksheets(sheetName)
    On Error GoTo 0

    If RequiredSheet Is Nothing Then
        Err.Raise vbObjectError + 2741, "ExcelVbaImportMvpExportHelper", "Missing required sheet: " & sheetName
    End If
End Function

Private Function RequiredHeaderColumn(ByVal ws As Worksheet, ByVal headerName As String) As Long
    Dim lastColumn As Long
    lastColumn = ws.Cells(1, ws.Columns.Count).End(xlToLeft).Column

    Dim columnIndex As Long
    For columnIndex = 1 To lastColumn
        If UCase$(Trim$(CStr(ws.Cells(1, columnIndex).Value))) = UCase$(headerName) Then
            RequiredHeaderColumn = columnIndex
            Exit Function
        End If
    Next columnIndex

    Err.Raise vbObjectError + 2742, "ExcelVbaImportMvpExportHelper", _
        "Missing required header: " & ws.Name & "." & headerName
End Function

Private Function LastDataRow(ByVal ws As Worksheet, ByVal rowIdColumn As Long) As Long
    LastDataRow = ws.Cells(ws.Rows.Count, rowIdColumn).End(xlUp).Row
End Function

Private Function JsonRowsArray(ByVal ws As Worksheet, ByVal requiredHeaders As Variant, ByRef headerColumns() As Long, ByVal lastRow As Long) As String
    Dim result As String
    result = "["

    Dim rowIndex As Long
    Dim rowWritten As Boolean
    rowWritten = False

    For rowIndex = 2 To lastRow
        If RowHasAnyValue(ws, headerColumns, rowIndex) Then
            If rowWritten Then
                result = result & ","
            End If
            result = result & JsonRowArray(ws, requiredHeaders, headerColumns, rowIndex)
            rowWritten = True
        End If
    Next rowIndex

    JsonRowsArray = result & "]"
End Function

Private Function JsonRowArray(ByVal ws As Worksheet, ByVal requiredHeaders As Variant, ByRef headerColumns() As Long, ByVal rowIndex As Long) As String
    Dim result As String
    result = "["

    Dim index As Long
    For index = LBound(requiredHeaders) To UBound(requiredHeaders)
        If index > LBound(requiredHeaders) Then
            result = result & ","
        End If
        result = result & JsonString(CStr(ws.Cells(rowIndex, headerColumns(index)).Value))
    Next index

    JsonRowArray = result & "]"
End Function

Private Function RowHasAnyValue(ByVal ws As Worksheet, ByRef headerColumns() As Long, ByVal rowIndex As Long) As Boolean
    Dim index As Long
    For index = LBound(headerColumns) To UBound(headerColumns)
        If Len(Trim$(CStr(ws.Cells(rowIndex, headerColumns(index)).Value))) > 0 Then
            RowHasAnyValue = True
            Exit Function
        End If
    Next index
End Function

Private Function JsonStringArray(ByVal values As Variant) As String
    Dim result As String
    result = "["

    Dim index As Long
    For index = LBound(values) To UBound(values)
        If index > LBound(values) Then
            result = result & ","
        End If
        result = result & JsonString(CStr(values(index)))
    Next index

    JsonStringArray = result & "]"
End Function

Private Function JsonPair(ByVal name As String, ByVal value As String) As String
    JsonPair = JsonString(name) & ":" & JsonString(value)
End Function

Private Function JsonString(ByVal value As String) As String
    Dim index As Long
    Dim codePoint As Integer
    Dim result As String
    result = """"

    For index = 1 To Len(value)
        codePoint = AscW(Mid$(value, index, 1))
        Select Case codePoint
            Case 34
                result = result & "\"""
            Case 92
                result = result & "\\"
            Case 8
                result = result & "\b"
            Case 9
                result = result & "\t"
            Case 10
                result = result & "\n"
            Case 12
                result = result & "\f"
            Case 13
                result = result & "\r"
            Case Else
                If codePoint < 32 Then
                    result = result & "\u" & Right$("0000" & Hex$(codePoint), 4)
                Else
                    result = result & Mid$(value, index, 1)
                End If
        End Select
    Next index

    JsonString = result & """"
End Function

Private Sub RequireAllowedExportExtension(ByVal targetPath As String)
    Dim lowerPath As String
    lowerPath = LCase$(targetPath)

    If Right$(lowerPath, 5) <> ".json" And Right$(lowerPath, 4) <> ".txt" Then
        Err.Raise vbObjectError + 2744, "ExcelVbaImportMvpExportHelper", _
            "Export target must end with .json or .txt."
    End If
End Sub

Private Sub WriteUtf8TextFile(ByVal targetPath As String, ByVal payload As String)
    On Error GoTo WriteFailed

    Dim fileNumber As Integer
    fileNumber = FreeFile
    Open targetPath For Output As #fileNumber
    Print #fileNumber, payload;
    Close #fileNumber
    Exit Sub

WriteFailed:
    On Error Resume Next
    If fileNumber <> 0 Then
        Close #fileNumber
    End If
    Err.Raise vbObjectError + 2745, "ExcelVbaImportMvpExportHelper", "Export file could not be written."
End Sub
